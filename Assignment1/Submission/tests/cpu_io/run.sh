#!/bin/bash

# Change to the Part2 directory
cd ./Assignment1/Submission/ || exit 1

./interrupts "tests/cpu_io/trace1.txt" "tests/cpu_io/vector_table.txt" "tests/cpu_io/output/execution1.txt" 
./interrupts "tests/cpu_io/trace2.txt" "tests/cpu_io/vector_table.txt" "tests/cpu_io/output/execution2.txt" 
./interrupts "tests/cpu_io/trace3.txt" "tests/cpu_io/vector_table.txt" "tests/cpu_io/output/execution3.txt"
./interrupts "tests/cpu_io/trace4.txt" "tests/cpu_io/vector_table.txt" "tests/cpu_io/output/execution4.txt"
