#include "scheduler.h"
#include "fcfs.h"
#include "priority.h"
#include "round_robin.h"

#define MAX_FILENAME 100

int main(int argc, char* argv[]) {
    const char* TYPE = "priority";  // Change this to switch schedulers
    char execution_filename[MAX_FILENAME];
    char memory_filename[MAX_FILENAME];
    
    if (argc != 2) {
        printf("Usage: %s <input_file>\n", argv[0]);
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
    if (strcmp(TYPE, "priority") == 0) {
        run_priority();
    } else if (strcmp(TYPE, "fcfs") == 0) {
        run_fcfs();
    } else if (strcmp(TYPE, "rr") == 0) {
        run_round_robin();
    }

    calculate_metrics();
    
    fclose(execution_log);
    fclose(memory_log);
    return 0;
}