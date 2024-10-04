#!/bin/bash

cd ./Assignment1/Submission/ || exit 1
 
./interrupts "tests/cpu_speed/trace_full_speed.txt" "tests/cpu_speed/vector_table.txt" "tests/cpu_speed/output/execution_full.txt" 
./interrupts "tests/cpu_speed/trace_half_speed.txt" "tests/cpu_speed/vector_table.txt" "tests/cpu_speed/output/execution_half.txt" 
./interrupts "tests/cpu_speed/trace_quarter_speed.txt" "tests/cpu_speed/vector_table.txt" "tests/cpu_speed/output/execution_quarter.txt"
./interrupts "tests/cpu_speed/trace_eigth_speed.txt" "tests/cpu_speed/vector_table.txt" "tests/cpu_speed/output/execution_eigth.txt"