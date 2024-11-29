#include "scheduler.h"
#include "fcfs.h"
#include "priority.h"
#include "round_robin.h"

int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("Usage: %s <input_file>\n", argv[0]);
        return 1;
    }

    init_memory_partitions();
    execution_log = fopen("execution_log.txt", "w");
    memory_log = fopen("memory_status.txt", "w");
    
    if (!execution_log || !memory_log) {
        printf("Error opening log files\n");
        return 1;
    }

    load_processes(argv[1]);
   run_priority();
   // run_fcfs();
   //run_round_robin();
    calculate_metrics();
    
    fclose(execution_log);
    fclose(memory_log);
    return 0;
}