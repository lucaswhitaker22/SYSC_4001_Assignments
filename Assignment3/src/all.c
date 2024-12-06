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
struct MemoryPartition memory_partitions[NUM_PARTITIONS];
struct PCB pcb_table[MAX_PROCESSES];
int num_processes = 0;
unsigned int current_time = 0;
struct PCB* running_process = NULL;
FILE *execution_log;
FILE *memory_log;
struct PCB* ready_queue[MAX_PROCESSES];
struct PCB* blocked_queue[MAX_PROCESSES];
int ready_queue_size = 0;
int blocked_queue_size = 0;

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
void run_fcfs(void);
void run_priority(void);
void run_round_robin(void);
void enqueue_process_priority(struct PCB* process);

// Implementation of functions
void init_memory_partitions(void) {
    unsigned int sizes[NUM_PARTITIONS] = {40, 25, 15, 10, 8, 2};
    for (int i = 0; i < NUM_PARTITIONS; i++) {
        memory_partitions[i].number = i + 1;
        memory_partitions[i].size = sizes[i];
        memory_partitions[i].occupied_by = -1;
    }
}

void log_transition(struct PCB* process, const char* old_state, const char* new_state) {
    fprintf(execution_log, "%u\t%d\t%s\t%s\n", current_time, process->pid, old_state, new_state);
    printf("Time: %u | PID: %d | %s -> %s\n", current_time, process->pid, old_state, new_state);
}

void log_memory_status(void) {
    unsigned int total_used = 0, total_free = 0, usable_free = 0;
    fprintf(memory_log, "%u\t", current_time);
    for (int i = 0; i < NUM_PARTITIONS; i++) {
        if (memory_partitions[i].occupied_by != -1) {
            total_used += memory_partitions[i].size;
        } else {
            total_free += memory_partitions[i].size;
            usable_free += memory_partitions[i].size;
        }
    }
    fprintf(memory_log, "%u\t", total_used);
    for (int i = 0; i < NUM_PARTITIONS; i++) {
        fprintf(memory_log, "%d", memory_partitions[i].occupied_by);
        if (i < NUM_PARTITIONS - 1) fprintf(memory_log, ",");
    }
    fprintf(memory_log, "\t%u\t%u\n", total_free, usable_free);
}

int find_memory_partition(unsigned int size) {
    int best_fit = -1;
    unsigned int smallest_suitable = 128;
    for (int i = 0; i < NUM_PARTITIONS; i++) {
        if (memory_partitions[i].occupied_by == -1 && memory_partitions[i].size >= size) {
            if (memory_partitions[i].size < smallest_suitable) {
                best_fit = i;
                smallest_suitable = memory_partitions[i].size;
            }
        }
    }
    return best_fit;
}

void load_processes(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        printf("Error opening input file\n");
        exit(1);
    }
    while (fscanf(file, "%u,%u,%u,%u,%u,%u", &pcb_table[num_processes].pid,
                  &pcb_table[num_processes].memory_size, &pcb_table[num_processes].arrival_time,
                  &pcb_table[num_processes].total_cpu_time, &pcb_table[num_processes].io_frequency,
                  &pcb_table[num_processes].io_duration) == 6) {
        pcb_table[num_processes].remaining_cpu_time = pcb_table[num_processes].total_cpu_time;
        pcb_table[num_processes].partition_number = -1;
        pcb_table[num_processes].last_io_time = 0;
        pcb_table[num_processes].priority = num_processes;
        pcb_table[num_processes].quantum_remaining = RR_QUANTUM;
        pcb_table[num_processes].completion_time = 0;
        pcb_table[num_processes].first_run_time = 0;
        strcpy(pcb_table[num_processes].state, STATE_NEW);
        num_processes++;
    }
    fclose(file);
}

void handle_io_completion(void) {
    for (int i = 0; i < blocked_queue_size; i++) {
        if ((current_time - blocked_queue[i]->last_io_time) >= blocked_queue[i]->io_duration) {
            strcpy(blocked_queue[i]->state, STATE_READY);
            ready_queue[ready_queue_size++] = blocked_queue[i];
            log_transition(blocked_queue[i], STATE_WAITING, STATE_READY);
            for (int j = i; j < blocked_queue_size - 1; j++) {
                blocked_queue[j] = blocked_queue[j + 1];
            }
            blocked_queue_size--;
            i--;
        }
    }
}

void enqueue_process(struct PCB* process) {
    ready_queue[ready_queue_size++] = process;
}

struct PCB* dequeue_process(void) {
    struct PCB* process = ready_queue[0];
    for (int i = 0; i < ready_queue_size - 1; i++) {
        ready_queue[i] = ready_queue[i + 1];
    }
    ready_queue_size--;
    return process;
}

int check_all_processes_done(void) {
    for (int i = 0; i < num_processes; i++) {
        if (strcmp(pcb_table[i].state, STATE_TERMINATED) != 0 &&
            (strcmp(pcb_table[i].state, STATE_NEW) != 0 || pcb_table[i].arrival_time <= current_time)) {
            return 0;
        }
    }
    return 1;
}

void handle_process_completion(struct PCB* process) {
    memory_partitions[process->partition_number].occupied_by = -1;
    strcpy(process->state, STATE_TERMINATED);
    log_transition(process, STATE_RUNNING, STATE_TERMINATED);
    log_memory_status();
    process->completion_time = current_time + 1;
}

void calculate_metrics(void) {
    unsigned int total_turnaround_time = 0;
    unsigned int total_wait_time = 0;
    unsigned int total_response_time = 0;
    for (int i = 0; i < num_processes; i++) {
        unsigned int turnaround_time = pcb_table[i].completion_time - pcb_table[i].arrival_time;
        unsigned int wait_time = turnaround_time - pcb_table[i].total_cpu_time;
        unsigned int response_time = pcb_table[i].first_run_time - pcb_table[i].arrival_time;
        total_turnaround_time += turnaround_time;
        total_wait_time += wait_time;
        total_response_time += response_time;
    }
    float avg_turnaround_time = (float)total_turnaround_time / num_processes;
    float avg_wait_time = (float)total_wait_time / num_processes;
    float avg_response_time = (float)total_response_time / num_processes;
    float throughput = (float)num_processes / current_time;
    printf("Average Turnaround Time: %.2f\n", avg_turnaround_time);
    printf("Average Wait Time: %.2f\n", avg_wait_time);
    printf("Average Response Time: %.2f\n", avg_response_time);
    printf("Throughput: %.2f processes/ms\n", throughput);
    fprintf(execution_log, "\nMetrics:\n");
    fprintf(execution_log, "Average Turnaround Time: %.2f\n", avg_turnaround_time);
    fprintf(execution_log, "Average Wait Time: %.2f\n", avg_wait_time);
    fprintf(execution_log, "Average Response Time: %.2f\n", avg_response_time);
    fprintf(execution_log, "Throughput: %.2f processes/ms\n", throughput);
}

void run_fcfs(void) {
    int all_processes_done = 0;
    while (!all_processes_done) {
        for (int i = 0; i < num_processes; i++) {
            if (pcb_table[i].arrival_time == current_time && strcmp(pcb_table[i].state, STATE_NEW) == 0) {
                int partition = find_memory_partition(pcb_table[i].memory_size);
                if (partition != -1) {
                    memory_partitions[partition].occupied_by = pcb_table[i].pid;
                    pcb_table[i].partition_number = partition;
                    enqueue_process(&pcb_table[i]);
                    log_transition(&pcb_table[i], STATE_NEW, STATE_READY);
                    strcpy(pcb_table[i].state, STATE_READY);
                    log_memory_status();
                }
            }
        }
        handle_io_completion();
        if (running_process == NULL && ready_queue_size > 0) {
            running_process = dequeue_process();
            strcpy(running_process->state, STATE_RUNNING);
            log_transition(running_process, STATE_READY, STATE_RUNNING);
            if (running_process->first_run_time == 0) {
                running_process->first_run_time = current_time;
            }
        }
        if (running_process != NULL) {
            running_process->remaining_cpu_time--;
            if (running_process->io_frequency > 0 &&
                (running_process->total_cpu_time - running_process->remaining_cpu_time) % running_process->io_frequency == 0 &&
                running_process->remaining_cpu_time > 0) {
                running_process->last_io_time = current_time;
                blocked_queue[blocked_queue_size++] = running_process;
                strcpy(running_process->state, STATE_WAITING);
                log_transition(running_process, STATE_RUNNING, STATE_WAITING);
                running_process = NULL;
            } else if (running_process->remaining_cpu_time == 0) {
                handle_process_completion(running_process);
                running_process = NULL;
            }
        }
        current_time++;
        all_processes_done = check_all_processes_done();
    }
}

void enqueue_process_priority(struct PCB* process) {
    int i = 0;
    while (i < ready_queue_size && ready_queue[i]->pid < process->pid) {
        i++;
    }
    for (int j = ready_queue_size; j > i; j--) {
        ready_queue[j] = ready_queue[j-1];
    }
    ready_queue[i] = process;
    ready_queue_size++;
}

void run_priority(void) {
    int all_processes_done = 0;
    while (!all_processes_done) {
        for (int i = 0; i < num_processes; i++) {
            if (pcb_table[i].arrival_time == current_time && strcmp(pcb_table[i].state, STATE_NEW) == 0) {
                int partition = find_memory_partition(pcb_table[i].memory_size);
                if (partition != -1) {
                    memory_partitions[partition].occupied_by = pcb_table[i].pid;
                    pcb_table[i].partition_number = partition;
                    enqueue_process_priority(&pcb_table[i]);
                    log_transition(&pcb_table[i], STATE_NEW, STATE_READY);
                    strcpy(pcb_table[i].state, STATE_READY);
                    log_memory_status();
                }
            }
        }
        handle_io_completion();
        if (running_process == NULL && ready_queue_size > 0) {
            running_process = dequeue_process();
            strcpy(running_process->state, STATE_RUNNING);
            log_transition(running_process, STATE_READY, STATE_RUNNING);
            if (running_process->first_run_time == 0) {
                running_process->first_run_time = current_time;
            }
        }
        if (running_process != NULL) {
            running_process->remaining_cpu_time--;
            if ((running_process->total_cpu_time - running_process->remaining_cpu_time) % running_process->io_frequency == 0 &&
                running_process->remaining_cpu_time > 0) {
                running_process->last_io_time = current_time;
                blocked_queue[blocked_queue_size++] = running_process;
                strcpy(running_process->state, STATE_WAITING);
                log_transition(running_process, STATE_RUNNING, STATE_WAITING);
                running_process = NULL;
            } else if (running_process->remaining_cpu_time == 0) {
                handle_process_completion(running_process);
                running_process = NULL;
            } else if (ready_queue_size > 0 && ready_queue[0]->priority < running_process->priority) {
                enqueue_process_priority(running_process);
                strcpy(running_process->state, STATE_READY);
                log_transition(running_process, STATE_RUNNING, STATE_READY);
                running_process = NULL;
            }
        }
        current_time++;
        all_processes_done = check_all_processes_done();
    }
}

void run_round_robin(void) {
    int all_processes_done = 0;
    int time_in_quantum = 0;
    while (!all_processes_done) {
        for (int i = 0; i < num_processes; i++) {
            if (pcb_table[i].arrival_time == current_time && strcmp(pcb_table[i].state, STATE_NEW) == 0) {
                int partition = find_memory_partition(pcb_table[i].memory_size);
                if (partition != -1) {
                    memory_partitions[partition].occupied_by = pcb_table[i].pid;
                    pcb_table[i].partition_number = partition;
                    enqueue_process(&pcb_table[i]);
                    log_transition(&pcb_table[i], STATE_NEW, STATE_READY);
                    strcpy(pcb_table[i].state, STATE_READY);
                    log_memory_status();
                }
            }
        }
        handle_io_completion();
        if (running_process != NULL) {
            running_process->remaining_cpu_time--;
            time_in_quantum++;
            if (running_process->io_frequency > 0 && (running_process->total_cpu_time - running_process->remaining_cpu_time) % running_process->io_frequency == 0 && running_process->remaining_cpu_time > 0) {
                running_process->last_io_time = current_time;
                blocked_queue[blocked_queue_size++] = running_process;
                strcpy(running_process->state, STATE_WAITING);
                log_transition(running_process, STATE_RUNNING, STATE_WAITING);
                running_process = NULL;
                time_in_quantum = 0;
            } else if (running_process->remaining_cpu_time == 0) {
                handle_process_completion(running_process);
                running_process = NULL;
                time_in_quantum = 0;
            } else if (time_in_quantum >= RR_QUANTUM) {
                enqueue_process(running_process);
                strcpy(running_process->state, STATE_READY);
                log_transition(running_process, STATE_RUNNING, STATE_READY);
                running_process = NULL;
                time_in_quantum = 0;
            }
        }
        if (running_process == NULL && ready_queue_size > 0) {
            running_process = dequeue_process();
            strcpy(running_process->state, STATE_RUNNING);
            log_transition(running_process, STATE_READY, STATE_RUNNING);
            if (running_process->first_run_time == 0) {
                running_process->first_run_time = current_time;
            }
            time_in_quantum = 0;
        }
        current_time++;
        all_processes_done = check_all_processes_done();
    }
}

void handle_process_completion(struct PCB* process) {
    memory_partitions[process->partition_number].occupied_by = -1;
    strcpy(process->state, STATE_TERMINATED);
    log_transition(process, STATE_RUNNING, STATE_TERMINATED);
    log_memory_status();
    process->completion_time = current_time + 1;
}

void calculate_metrics(void) {
    unsigned int total_turnaround_time = 0;
    unsigned int total_wait_time = 0;
    unsigned int total_response_time = 0;

    for (int i = 0; i < num_processes; i++) {
        unsigned int turnaround_time = pcb_table[i].completion_time - pcb_table[i].arrival_time;
        unsigned int wait_time = turnaround_time - pcb_table[i].total_cpu_time;
        unsigned int response_time = pcb_table[i].first_run_time - pcb_table[i].arrival_time;
        
        total_turnaround_time += turnaround_time;
        total_wait_time += wait_time;
        total_response_time += response_time;
    }

    float avg_turnaround_time = (float)total_turnaround_time / num_processes;
    float avg_wait_time = (float)total_wait_time / num_processes;
    float avg_response_time = (float)total_response_time / num_processes;
    float throughput = (float)num_processes / current_time;

    printf("Average Turnaround Time: %.2f\n", avg_turnaround_time);
    printf("Average Wait Time: %.2f\n", avg_wait_time);
    printf("Average Response Time: %.2f\n", avg_response_time);
    printf("Throughput: %.2f processes/ms\n", throughput);

    fprintf(execution_log, "\nMetrics:\n");
    fprintf(execution_log, "Average Turnaround Time: %.2f\n", avg_turnaround_time);
    fprintf(execution_log, "Average Wait Time: %.2f\n", avg_wait_time);
    fprintf(execution_log, "Average Response Time: %.2f\n", avg_response_time);
    fprintf(execution_log, "Throughput: %.2f processes/ms\n", throughput);
}