#include "scheduler.h"
#include "fcfs.h"
#include "priority.h"
#include "round_robin.h"

#define MAX_FILENAME 100

int main(int argc, char* argv[]) {
    char execution_filename[MAX_FILENAME];
    char memory_filename[MAX_FILENAME];
    const char* TYPE;
    
    if (argc != 3) {
        printf("Usage: %s <input_file> <scheduler_type>\n", argv[0]);
        printf("Scheduler types: FCFS, RR, EP\n");
        return 1;
    }

    // Set TYPE based on the command-line argument
    if (strcasecmp(argv[2], "FCFS") == 0) {
        TYPE = "FCFS";
    } else if (strcasecmp(argv[2], "RR") == 0) {
        TYPE = "RR";
    } else if (strcasecmp(argv[2], "EP") == 0) {
        TYPE = "EP";
    } else {
        printf("Invalid scheduler type. Use FCFS, RR, or EP.\n");
        return 1;
    }

    // Create filenames using TYPE
    snprintf(execution_filename, MAX_FILENAME, "execution_log_%s.txt", TYPE);
    snprintf(memory_filename, MAX_FILENAME, "memory_status_%s.txt", TYPE);

    init_memory_partitions();
    execution_log = fopen(execution_filename, "w");
    memory_log = fopen(memory_filename, "w");
    
    if (!execution_log || !memory_log) {
        printf("Error opening log files\n");
        return 1;
    }

    load_processes(argv[1]);

    // Choose scheduler based on TYPE
    if (strcmp(TYPE, "EP") == 0) {
        run_priority();
    } else if (strcmp(TYPE, "FCFS") == 0) {
        run_fcfs();
    } else if (strcmp(TYPE, "RR") == 0) {
        run_round_robin();
    }

    calculate_metrics();
    
    fclose(execution_log);
    fclose(memory_log);
    return 0;
}