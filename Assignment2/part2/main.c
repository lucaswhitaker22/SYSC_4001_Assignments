// main.c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <time.h>
#include <errno.h>

#define SHM_SIZE sizeof(struct shared_data)

struct shared_data {
    int random_number;
    int process2_active;
};

union semun {
    int val;
    struct semid_ds *buf;
    unsigned short *array;
};

// Fixed: Added proper semaphore operation
void sem_operation(int semid, int op) {
    struct sembuf sb = {0, op, 0};
    semop(semid, &sb, 1);
}

int init_semaphore() {
    int semid = semget(IPC_PRIVATE, 1, IPC_CREAT | 0666);
    
    union semun arg;
    arg.val = 1;
    semctl(semid, 0, SETVAL, arg);
    
    return semid;
}

int init_shared_memory(struct shared_data **shared) {
    int shmid = shmget(IPC_PRIVATE, SHM_SIZE, IPC_CREAT | 0666);
    
    *shared = (struct shared_data *)shmat(shmid, NULL, 0);
    
    (*shared)->random_number = -1;
    (*shared)->process2_active = 0;
    
    return shmid;
}

void process2(struct shared_data *shared, int semid) {
    printf("Process 2 started (PID: %d)\n", getpid());
    while(1) {
        sem_operation(semid, -1);  // Wait

        if (!shared->process2_active) {
            printf("\nProcess 2: Waiting for activation (current value: %d)\n", shared->random_number);
        } else {
            printf("\nProcess 2: Reading shared value %d\n", shared->random_number);
            printf("Process 2: Reading a %s value\n", 
                   shared->random_number > 5 ? "High" : "Low");
        }

        if (shared->random_number == 0) {
            printf("Process 2: Terminating (random number is 0)\n");
            sem_operation(semid, 1);
            break;
        }

        sem_operation(semid, 1);
        sleep(1);
    }
}

void process1(struct shared_data *shared, int semid) {
    printf("Process 1 started (PID: %d)\n", getpid());
    srand(time(NULL));
    while(1) {
        sem_operation(semid, -1);
        
        shared->random_number = rand() % 11;
        printf("\nProcess 1: Generated number %d\n", shared->random_number);

        if (shared->random_number < 5) {
            shared->process2_active = 1;
            printf("Process 1: Activating Process 2 (value < 5)\n");
        }

        sem_operation(semid, 1);

        if (shared->random_number == 0) {
            printf("Process 1: Terminating (random number is 0)\n");
            break;
        }
        sleep(1);
    }
}

int main() {
    struct shared_data *shared;
    int shmid, semid;
    pid_t pid;

    shmid = init_shared_memory(&shared);
    semid = init_semaphore();

    printf("Starting processes...\n");

    pid = fork();
    if (pid == 0) {
        process2(shared, semid);
        exit(0);
    } else {
        process1(shared, semid);

        // Wait for child process to finish
        wait(NULL);
        
        // Cleanup resources
        shmdt(shared);
        shmctl(shmid, IPC_RMID, NULL);
        semctl(semid, 0, IPC_RMID);

        printf("\nAll processes terminated and resources cleaned up\n");
    }
    return 0;
}