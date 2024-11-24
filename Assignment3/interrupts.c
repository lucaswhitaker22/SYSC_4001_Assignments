#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAX_PROCESSES 100
#define NUM_PARTITIONS 6
#define STATE_NEW "NEW"
#define STATE_READY "READY"
#define STATE_RUNNING "RUNNING"
#define STATE_WAITING "WAITING"
#define STATE_TERMINATED "TERMINATED"

// Memory Partition structure
struct MemoryPartition {
    unsigned int number;
    unsigned int size;
    int occupied_by;  // -1 for free, otherwise contains PID
};

// Process Control Block (PCB) structure
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
    unsigned int priority;  // For priority scheduling
    unsigned int quantum_remaining;  // For Round Robin
};

// Global variables
struct MemoryPartition memory_partitions[NUM_PARTITIONS];
struct PCB pcb_table[MAX_PROCESSES];
int num_processes = 0;
unsigned int current_time = 0;
struct PCB* running_process = NULL;
FILE *execution_log;
FILE *memory_log;

// Ready and blocked queues
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
    fprintf(execution_log, "%02d\t%d\t%s\t%s\n", 
            current_time, process->pid, old_state, new_state);
}

void log_memory_status(void) {
    unsigned int total_used = 0;
    unsigned int total_free = 0;
    unsigned int usable_free = 0;
    
    fprintf(memory_log, "%d\t", current_time);
    
    // Calculate memory usage
    for (int i = 0; i < NUM_PARTITIONS; i++) {
        if (memory_partitions[i].occupied_by != -1) {
            total_used += memory_partitions[i].size;
        } else {
            total_free += memory_partitions[i].size;
            usable_free += memory_partitions[i].size;
        }
    }
    
    fprintf(memory_log, "%u\t", total_used);
    
    // Print partition state
    for (int i = 0; i < NUM_PARTITIONS; i++) {
        fprintf(memory_log, "%d", memory_partitions[i].occupied_by);
        if (i < NUM_PARTITIONS - 1) fprintf(memory_log, ",");
    }
    
    fprintf(memory_log, "\t%u\t%u\n", total_free, usable_free);
}

int find_memory_partition(unsigned int size) {
    for (int i = 0; i < NUM_PARTITIONS; i++) {
        if (memory_partitions[i].occupied_by == -1 && 
            memory_partitions[i].size >= size) {
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
        strcpy(pcb_table[num_processes].state, STATE_NEW);
        num_processes++;
    }
    fclose(file);
}

void run_fcfs(void) {
    int all_processes_done = 0;
    
    while (!all_processes_done) {
        // Check for new arrivals
        for (int i = 0; i < num_processes; i++) {
            if (pcb_table[i].arrival_time == current_time && 
                strcmp(pcb_table[i].state, STATE_NEW) == 0) {
                
                int partition = find_memory_partition(pcb_table[i].memory_size);
                if (partition != -1) {
                    memory_partitions[partition].occupied_by = pcb_table[i].pid;
                    pcb_table[i].partition_number = partition;
                    ready_queue[ready_queue_size++] = &pcb_table[i];
                    log_transition(&pcb_table[i], STATE_NEW, STATE_READY);
                    strcpy(pcb_table[i].state, STATE_READY);
                    log_memory_status();
                }
            }
        }

        // Handle I/O completion
        for (int i = 0; i < blocked_queue_size; i++) {
            if ((current_time - blocked_queue[i]->last_io_time) >= 
                blocked_queue[i]->io_duration) {
                strcpy(blocked_queue[i]->state, STATE_READY);
                ready_queue[ready_queue_size++] = blocked_queue[i];
                log_transition(blocked_queue[i], STATE_WAITING, STATE_READY);
                
                // Remove from blocked queue
                for (int j = i; j < blocked_queue_size - 1; j++) {
                    blocked_queue[j] = blocked_queue[j + 1];
                }
                blocked_queue_size--;
                i--;
            }
        }

        // Start new process if CPU is free
        if (running_process == NULL && ready_queue_size > 0) {
            running_process = ready_queue[0];
            // Shift queue
            for (int i = 0; i < ready_queue_size - 1; i++) {
                ready_queue[i] = ready_queue[i + 1];
            }
            ready_queue_size--;
            strcpy(running_process->state, STATE_RUNNING);
            log_transition(running_process, STATE_READY, STATE_RUNNING);
        }

        // Handle running process
        if (running_process != NULL) {
            running_process->remaining_cpu_time--;
            
            // Check for I/O
            if (running_process->remaining_cpu_time > 0 && 
                (current_time - running_process->last_io_time) >= 
                running_process->io_frequency) {
                
                running_process->last_io_time = current_time;
                blocked_queue[blocked_queue_size++] = running_process;
                strcpy(running_process->state, STATE_WAITING);
                log_transition(running_process, STATE_RUNNING, STATE_WAITING);
                running_process = NULL;
            }
            // Check for completion
            else if (running_process->remaining_cpu_time == 0) {
                memory_partitions[running_process->partition_number].occupied_by = -1;
                strcpy(running_process->state, STATE_TERMINATED);
                log_transition(running_process, STATE_RUNNING, STATE_TERMINATED);
                log_memory_status();
                running_process = NULL;
            }
        }

        current_time++;
        
        // Check if all processes are done
        all_processes_done = 1;
        for (int i = 0; i < num_processes; i++) {
            if (strcmp(pcb_table[i].state, STATE_TERMINATED) != 0) {
                all_processes_done = 0;
                break;
            }
        }
    }
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("Usage: %s <input_file>\n", argv[0]);
        return 1;
    }

    // Initialize memory partitions
    init_memory_partitions();

    // Open log files
    execution_log = fopen("execution_log.txt", "w");
    memory_log = fopen("memory_status.txt", "w");
    
    if (!execution_log || !memory_log) {
        printf("Error opening log files\n");
        return 1;
    }

    // Load and run processes
    load_processes(argv[1]);
    run_fcfs();

    // Cleanup
    fclose(execution_log);
    fclose(memory_log);

    return 0;
}