#include "scheduler.h"

// Global variable definitions
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
    for (int i = 0; i < NUM_PARTITIONS; i++) {
        if (memory_partitions[i].occupied_by == -1 && memory_partitions[i].size >= size) {
            return i;
        }
    }
    return -1;
}

void load_processes(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        printf("Error opening input file\n");
        exit(1);
    }
    
    while (fscanf(file, "%u,%u,%u,%u,%u,%u", 
           &pcb_table[num_processes].pid,
           &pcb_table[num_processes].memory_size,
           &pcb_table[num_processes].arrival_time,
           &pcb_table[num_processes].total_cpu_time,
           &pcb_table[num_processes].io_frequency,
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
        if (strcmp(pcb_table[i].state, STATE_TERMINATED) != 0) {
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