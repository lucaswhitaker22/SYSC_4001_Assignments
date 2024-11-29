#include "priority.h"

extern struct PCB* ready_queue[MAX_PROCESSES];
extern struct PCB* blocked_queue[MAX_PROCESSES];
extern int ready_queue_size;
extern int blocked_queue_size;
extern struct PCB* running_process;
extern unsigned int current_time;
extern int num_processes;
extern struct MemoryPartition memory_partitions[NUM_PARTITIONS];
extern struct PCB pcb_table[MAX_PROCESSES];

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
        // Check for new arrivals
        for (int i = 0; i < num_processes; i++) {
            if (pcb_table[i].arrival_time == current_time && 
                strcmp(pcb_table[i].state, STATE_NEW) == 0) {
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

            // Check for I/O
            if ((running_process->total_cpu_time - running_process->remaining_cpu_time) 
                % running_process->io_frequency == 0 && 
                running_process->remaining_cpu_time > 0) {
                running_process->last_io_time = current_time;
                blocked_queue[blocked_queue_size++] = running_process;
                strcpy(running_process->state, STATE_WAITING);
                log_transition(running_process, STATE_RUNNING, STATE_WAITING);
                running_process = NULL;
            }
            // Check for process completion
            else if (running_process->remaining_cpu_time == 0) {
                handle_process_completion(running_process);
                running_process = NULL;
            }
            // Check for higher priority process
            else if (ready_queue_size > 0 && 
                     ready_queue[0]->priority < running_process->priority) {
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