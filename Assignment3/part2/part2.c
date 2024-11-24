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

#define NUM_TAS 5
#define NUM_STUDENTS 20
#define NUM_ROUNDS 3
#define SHM_NAME "/student_list"

typedef struct {
    int student_list[NUM_STUDENTS];
    int current_index;
} SharedData;

sem_t *semaphores[NUM_TAS];
const char *SEM_NAMES[] = {"/sem1", "/sem2", "/sem3", "/sem4", "/sem5"};

int try_acquire_semaphores(int ta_id) {
    int next_sem = (ta_id + 1) % NUM_TAS;
    
    if (sem_trywait(semaphores[ta_id]) == 0) {
        if (sem_trywait(semaphores[next_sem]) == 0) {
            return 1;
        }
        sem_post(semaphores[ta_id]);
        return 0;
    }
    return 0;
}

void ta_process(int ta_id, SharedData *shared_data) {
    int rounds = 0;
    
    while (rounds < NUM_ROUNDS) {
        int next_sem = (ta_id + 1) % NUM_TAS;
        
        // Try to acquire both semaphores
        while (!try_acquire_semaphores(ta_id)) {
            printf("TA %d failed to acquire semaphores %d and %d, retrying...\n", 
                   ta_id + 1, ta_id + 1, next_sem + 1);
            usleep(100000); // Sleep for 100ms before retrying
        }
        
        printf("TA %d acquired semaphores %d and %d\n", 
               ta_id + 1, ta_id + 1, next_sem + 1);
        
        // Critical section - access shared memory
        int student_num = shared_data->student_list[shared_data->current_index];
        printf("TA %d reading student %d at index %d\n", 
               ta_id + 1, student_num, shared_data->current_index);
        shared_data->current_index = (shared_data->current_index + 1) % NUM_STUDENTS;
        
        // Random delay for database access (1-4 seconds)
        sleep(rand() % 4 + 1);
        
        // Release semaphores
        sem_post(semaphores[ta_id]);
        sem_post(semaphores[next_sem]);
        printf("TA %d released semaphores\n", ta_id + 1);
        
        // Check if end of list
        if (student_num == 9999) {
            printf("TA %d reached end of list, starting new round\n", ta_id + 1);
            shared_data->current_index = 0;
            rounds++;
            if (rounds >= NUM_ROUNDS) {
                printf("TA %d completed all rounds\n", ta_id + 1);
                break;
            }
            continue;
        }
        
        // Marking process
        printf("TA %d marking student %d\n", ta_id + 1, student_num);
        int mark = rand() % 11;
        printf("TA %d gave student %d mark: %d\n", ta_id + 1, student_num, mark);
        
        // Simulate marking time (1-10 seconds)
        sleep(rand() % 10 + 1);
    }
    
    exit(0);
}

int main() {
    srand(time(NULL));
    
    // Create shared memory
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
    
    // Initialize shared data
    shared_data->current_index = 0;
    for (int i = 0; i < NUM_STUDENTS - 1; i++) {
        shared_data->student_list[i] = i + 1;
    }
    shared_data->student_list[NUM_STUDENTS - 1] = 9999;
    
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