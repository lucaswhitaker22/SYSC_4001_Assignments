#ifndef PRIORITY_H
#define PRIORITY_H

#include "scheduler.h"
// External variable declarations
extern struct MemoryPartition memory_partitions[NUM_PARTITIONS];
extern struct PCB pcb_table[MAX_PROCESSES];
extern int num_processes;
extern unsigned int current_time;
extern struct PCB* ready_queue[MAX_PROCESSES];
extern struct PCB* blocked_queue[MAX_PROCESSES];
extern int ready_queue_size;
extern int blocked_queue_size;
extern FILE *execution_log;
extern FILE *memory_log;
void run_priority(void);
void enqueue_process_priority(struct PCB* process);
struct PCB* dequeue_process(void);
#endif