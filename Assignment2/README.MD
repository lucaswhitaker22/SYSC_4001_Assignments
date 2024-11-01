# Part 2
1. Navigate to part2 directory
2. Compile and run ```main.c```

### Example Output:
```Starting processes...
Process 1 started (PID: 9517)

Process 1: Generated number 4
Process 1: Activating Process 2 (value < 5)
Process 2 started (PID: 9518)

Process 2: Reading shared value 4
Process 2: Reading a Low value

Process 1: Generated number 10

Process 2: Reading shared value 10
Process 2: Reading a High value

Process 1: Generated number 9

Process 2: Reading shared value 9
Process 2: Reading a High value

Process 1: Generated number 3
Process 1: Activating Process 2 (value < 5)

Process 2: Reading shared value 3
Process 2: Reading a Low value

Process 1: Generated number 10

Process 2: Reading shared value 10
Process 2: Reading a High value

Process 1: Generated number 9

Process 2: Reading shared value 9
Process 2: Reading a High value

Process 1: Generated number 6

Process 2: Reading shared value 6
Process 2: Reading a High value

Process 1: Generated number 10

Process 2: Reading shared value 10
Process 2: Reading a High value

Process 1: Generated number 4
Process 1: Activating Process 2 (value < 5)

Process 2: Reading shared value 4
Process 2: Reading a Low value

Process 1: Generated number 1
Process 1: Activating Process 2 (value < 5)

Process 2: Reading shared value 1
Process 2: Reading a Low value

Process 1: Generated number 10

Process 2: Reading shared value 10
Process 2: Reading a High value

Process 1: Generated number 0
Process 1: Activating Process 2 (value < 5)
Process 1: Terminating (random number is 0)

Process 2: Reading shared value 0
Process 2: Reading a Low value
Process 2: Terminating (random number is 0)

All processes terminated and resources cleaned up
```
# Part 3

### Instructions:
1. Compile interrupts.cpp
2. Run the program with command
```./interrupts <PATH TO INPUT DIRECTORY>```
3. For running tests, navigate to the /outputs/ directory (or wherever interrupt.cpp was built to)
4. Then, run the command ```./interrupts ../tests/<TEST NUMBER>```

### Test Results:
- Test results are written to the /tests/<TEST NUMBER>/outputs/ folder