#include <stdio.h>
#include <time.h>

// Function test to simulate a CPU-intensive task
void cpu_intensive_task() {
    volatile long long sum = 0; 
    for (long long i = 0; i < 1000000000; i++) {
        sum += i; 
    }
    printf("CPU Intensive Task Completed. Sum: %lld\n", sum);
}

int main() {
    clock_t start_time, end_time;
    double cpu_time_used;

    start_time = clock();  //Start time measure
    
    cpu_intensive_task();  //Call
    
    end_time = clock();  
    cpu_time_used = ((double)(end_time - start_time)) / CLOCKS_PER_SEC;  // Calculate time taken

    printf("Time taken for CPU intensive task: %f seconds\n", cpu_time_used);
    return 0;
}

