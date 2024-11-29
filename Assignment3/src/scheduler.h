#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include <io.h>
#define access _access
#else
#include <unistd.h>
#endif

#define MAX_PROCESSES 100
#define NUM_PARTITIONS 6
#define RR_QUANTUM 100

// State definitions
#define STATE_NEW "NEW"
#define STATE_READY "READY"
#define STATE_RUNNING "RUNNING"
#define STATE_WAITING "WAITING"
#define STATE_TERMINATED "TERMINATED"

// Struct definitions
struct MemoryPartition {
    unsigned int number;
    unsigned int size;
    int occupied_by;
};

struct PCB {
    unsigned int pid;
    unsigned int memory_size;
    unsigned int arrival_time;
    unsigned int total_cpu_time;
    unsigned int remaining_cpu_time;
    unsigned int io_frequency;
    unsigned int io_duration;
    char state[20];
    int partition_number;
    unsigned int last_io_time;
    unsigned int priority;
    unsigned int quantum_remaining;
    unsigned int completion_time;
    unsigned int first_run_time;
};

// Global variable declarations
extern struct MemoryPartition memory_partitions[NUM_PARTITIONS];
extern struct PCB pcb_table[MAX_PROCESSES];
extern int num_processes;
extern unsigned int current_time;
extern struct PCB* running_process;
extern FILE *execution_log;
extern FILE *memory_log;
extern struct PCB* ready_queue[MAX_PROCESSES];
extern struct PCB* blocked_queue[MAX_PROCESSES];
extern int ready_queue_size;
extern int blocked_queue_size;

// Function declarations
void init_memory_partitions(void);
void log_transition(struct PCB* process, const char* old_state, const char* new_state);
void log_memory_status(void);
int find_memory_partition(unsigned int size);
void load_processes(const char* filename);
void handle_io_completion(void);
void enqueue_process(struct PCB* process);
struct PCB* dequeue_process(void);
int check_all_processes_done(void);
void handle_process_completion(struct PCB* process);
void calculate_metrics(void);

#endif