#include <stdio.h>
#include <time.h>
#include <unistd.h> // for sleep

int main() {
    clock_t start_time = clock();

    // Simulate frequent interrupts
    for (int i = 0; i < 1000000; i++) {
        // Simulate an interrupt every 1000 iterations
        if (i % 1000 == 0) {
            printf("Frequent interrupt at iteration %d\n", i);
        }
    }

    clock_t end_time = clock();
    double time_taken = (double)(end_time - start_time) / CLOCKS_PER_SEC;

    printf("Frequent Interrupt Test Completed.\n");
    printf("Time taken: %f seconds\n", time_taken);

    return 0;
}
