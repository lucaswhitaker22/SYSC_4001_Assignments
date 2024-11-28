#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <time.h>
#include <string.h>
#include <errno.h>


#define MAX_RETRIES 3
#define MIN_BACKOFF_TIME 100000  // 100ms in microseconds
#define MAX_BACKOFF_TIME 1000000 
#define NUM_TAS 5
#define NUM_LINES 20
#define STUDENTS_PER_LINE 4
#define NUM_ROUNDS 3
#define SHM_NAME "/student_list.txt"

// Add this structure to track semaphore states
typedef struct {
    sem_t *mutex;
    int owner;
    int locked;
} ResourceSemaphore;

ResourceSemaphore *resource_semaphores[NUM_TAS];

typedef struct {
    int student_list[NUM_LINES][STUDENTS_PER_LINE];
    int current_line;
    int current_pos;
    int ta_progress[NUM_TAS];  // Track each TA's progress
} SharedData;

sem_t *semaphores[NUM_TAS];
const char *SEM_NAMES[] = {"/sem1", "/sem2", "/sem3", "/sem4", "/sem5"};

// Function to write marks to individual TA files
void write_mark_to_file(int ta_id, int student_num, int mark) {
    char filename[20];
    snprintf(filename, sizeof(filename), "TA%d.txt", ta_id + 1);
    FILE *fp = fopen(filename, "a");
    if (fp) {
        fprintf(fp, "Student: %04d, Mark: %d\n", student_num, mark);
        fclose(fp);
    }
}

int try_acquire_semaphores(int ta_id) {
    int next_sem = (ta_id + 1) % NUM_TAS;
    
    if (sem_trywait(semaphores[ta_id]) == 0) {
        if (sem_trywait(semaphores[next_sem]) == 0) {
            return 1;
        }
        sem_post(semaphores[ta_id]);
    }
    return 0;
}

int acquire_semaphores_safely(int ta_id) {
    int first_sem = ta_id;
    int second_sem = (ta_id + 1) % NUM_TAS;
    
    // Always acquire lower numbered semaphore first
    if (first_sem > second_sem) {
        int temp = first_sem;
        first_sem = second_sem;
        second_sem = temp;
    }
    
    int retries = 0;
    while (retries < MAX_RETRIES) {
        if (sem_trywait(semaphores[first_sem]) == 0) {
            if (sem_trywait(semaphores[second_sem]) == 0) {
                return 1;
            }
            sem_post(semaphores[first_sem]);
            
            // Exponential backoff
            unsigned int backoff_time = MIN_BACKOFF_TIME * (1 << retries);
            if (backoff_time > MAX_BACKOFF_TIME) backoff_time = MAX_BACKOFF_TIME;
            usleep(rand() % backoff_time);
        }
        retries++;
    }
    return 0;
}

// Modified TA process function
void ta_process(int ta_id, SharedData *shared_data) {
    char filename[20];
    snprintf(filename, sizeof(filename), "TA%d.txt", ta_id + 1);
    FILE *fp = fopen(filename, "w");
    if (fp) fclose(fp);
    
    int rounds = 0;
    
    while (rounds < NUM_ROUNDS) {
        // Try to acquire resources with exponential backoff
        while (!acquire_semaphores_safely(ta_id)) {
            printf("TA %d backing off and retrying\n", ta_id + 1);
            usleep(rand() % MAX_BACKOFF_TIME);
        }
        
        // Critical section
        int student_num;
        {
            student_num = shared_data->student_list[shared_data->current_line][shared_data->current_pos];
            printf("TA %d reading student %04d\n", ta_id + 1, student_num);
            
            shared_data->current_pos++;
            if (shared_data->current_pos >= STUDENTS_PER_LINE) {
                shared_data->current_pos = 0;
                shared_data->current_line = (shared_data->current_line + 1) % NUM_LINES;
            }
        }
        
        // Release resources immediately after critical section
        int first_sem = ta_id;
        int second_sem = (ta_id + 1) % NUM_TAS;
        if (first_sem > second_sem) {
            int temp = first_sem;
            first_sem = second_sem;
            second_sem = temp;
        }
        sem_post(semaphores[second_sem]);
        sem_post(semaphores[first_sem]);
        
        // Rest of the process remains the same
        if (student_num == 9999) {
            printf("TA %d completed round %d\n", ta_id + 1, rounds + 1);
            rounds++;
            if (rounds >= NUM_ROUNDS) break;
            continue;
        }
        
        int mark = rand() % 11;
        write_mark_to_file(ta_id, student_num, mark);
        sleep(rand() % 10 + 1);
    }
    exit(0);
}
int main() {
    srand(time(NULL));
    
    // Create and initialize shared memory
    int shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
    if (shm_fd == -1) {
        perror("shm_open failed");
        exit(1);
    }
    
    if (ftruncate(shm_fd, sizeof(SharedData)) == -1) {
        perror("ftruncate failed");
        exit(1);
    }
    
    SharedData *shared_data = mmap(NULL, sizeof(SharedData), 
                                 PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (shared_data == MAP_FAILED) {
        perror("mmap failed");
        exit(1);
    }
    
    // Initialize shared data from file
    FILE *student_list = fopen("student_list.txt", "r");
    if (!student_list) {
        perror("Failed to open student list");
        exit(1);
    }
    
    shared_data->current_line = 0;
    shared_data->current_pos = 0;
    memset(shared_data->ta_progress, 0, sizeof(shared_data->ta_progress));
    
    for (int i = 0; i < NUM_LINES; i++) {
        for (int j = 0; j < STUDENTS_PER_LINE; j++) {
            if (fscanf(student_list, "%d", &shared_data->student_list[i][j]) != 1) {
                fprintf(stderr, "Error reading student list\n");
                exit(1);
            }
        }
    }
    fclose(student_list);
    
    // Initialize semaphores
    for (int i = 0; i < NUM_TAS; i++) {
        sem_unlink(SEM_NAMES[i]);
        semaphores[i] = sem_open(SEM_NAMES[i], O_CREAT, 0644, 1);
        if (semaphores[i] == SEM_FAILED) {
            perror("sem_open failed");
            exit(1);
        }
    }
    
    // Create TA processes
    pid_t pids[NUM_TAS];
    for (int i = 0; i < NUM_TAS; i++) {
        pids[i] = fork();
        if (pids[i] == -1) {
            perror("fork failed");
            exit(1);
        }
        if (pids[i] == 0) {
            ta_process(i, shared_data);
        }
    }
    
    // Wait for all TAs to finish
    for (int i = 0; i < NUM_TAS; i++) {
        waitpid(pids[i], NULL, 0);
    }
    
    // Cleanup
    for (int i = 0; i < NUM_TAS; i++) {
        sem_close(semaphores[i]);
        sem_unlink(SEM_NAMES[i]);
    }
    
    munmap(shared_data, sizeof(SharedData));
    shm_unlink(SHM_NAME);
    
    return 0;
}