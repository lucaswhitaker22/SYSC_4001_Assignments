#include "round_robin.h"

extern struct PCB* ready_queue[MAX_PROCESSES];
extern struct PCB* blocked_queue[MAX_PROCESSES];
extern int ready_queue_size;
extern int blocked_queue_size;
extern struct PCB* running_process;
extern unsigned int current_time;
extern int num_processes;
extern struct MemoryPartition memory_partitions[NUM_PARTITIONS];
extern struct PCB pcb_table[MAX_PROCESSES];
void run_round_robin(void) {
    int all_processes_done = 0;
    int time_in_quantum = 0;
    
    while (!all_processes_done) {
        // Check for new arrivals
        for (int i = 0; i < num_processes; i++) {
            if (pcb_table[i].arrival_time == current_time && 
                strcmp(pcb_table[i].state, STATE_NEW) == 0) {
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

        // Execute current process
        if (running_process != NULL) {
            running_process->remaining_cpu_time--;
            time_in_quantum++;

            // Handle I/O
            if (running_process->io_frequency > 0 && 
                (running_process->total_cpu_time - running_process->remaining_cpu_time) 
                % running_process->io_frequency == 0 && 
                running_process->remaining_cpu_time > 0) {
                running_process->last_io_time = current_time;
                blocked_queue[blocked_queue_size++] = running_process;
                strcpy(running_process->state, STATE_WAITING);
                log_transition(running_process, STATE_RUNNING, STATE_WAITING);
                running_process = NULL;
                time_in_quantum = 0;
            }
            // Handle completion
            else if (running_process->remaining_cpu_time == 0) {
                handle_process_completion(running_process);
                running_process = NULL;
                time_in_quantum = 0;
            }
            // Check quantum expiration
            else if (time_in_quantum >= RR_QUANTUM) {
                enqueue_process(running_process);
                strcpy(running_process->state, STATE_READY);
                log_transition(running_process, STATE_RUNNING, STATE_READY);
                running_process = NULL;
                time_in_quantum = 0;
            }
        }

        // Select new process if needed
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