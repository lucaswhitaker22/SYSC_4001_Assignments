#include <stdio.h>
#include <time.h>

int main() {
    FILE *file = fopen("output.txt", "w");
    if (file == NULL) {
        perror("Failed to open file");
        return 1;
    }

    clock_t start_time = clock();

    // I/O-intensive task: Writing to a file
    for (long long i = 0; i < 1000000; i++) {
        fprintf(file, "This is line number %lld\n", i);
    }

    fclose(file);
    clock_t end_time = clock();
    double time_taken = (double)(end_time - start_time) / CLOCKS_PER_SEC;

    printf("I/O Intensive Task Completed.\n");
    printf("Time taken for I/O intensive task: %f seconds\n", time_taken);

    return 0;
}
