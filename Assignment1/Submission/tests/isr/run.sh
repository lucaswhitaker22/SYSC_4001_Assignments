#!/bin/bash


# Change to the Part2 directory
cd ./Assignment1/Submission/ || exit 1

./interrupts "tests/isr/trace_2.txt" "tests/isr/vector_table.txt" "tests/isr/output/execution_2.txt" 
./interrupts "tests/isr/trace_3.txt" "tests/isr/vector_table.txt" "tests/isr/output/execution_3.txt" 
./interrupts "tests/isr/trace_4.txt" "tests/isr/vector_table.txt" "tests/isr/output/execution_4.txt" 
./interrupts "tests/isr/trace_5.txt" "tests/isr/vector_table.txt" "tests/isr/output/execution_5.txt" 
