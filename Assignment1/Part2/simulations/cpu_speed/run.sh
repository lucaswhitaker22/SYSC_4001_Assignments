#!/bin/bash

# Change to the Part2 directory
cd ./Assignment1/Part2

./interrupts "simulations/cpu_speed/trace_full_speed.txt" "simulations/cpu_speed/vector_table.txt" "simulations/cpu_speed/output/execution_full.txt" 
./interrupts "simulations/cpu_speed/trace_half_speed.txt" "simulations/cpu_speed/vector_table.txt" "simulations/cpu_speed/output/execution_half.txt" 
./interrupts "simulations/cpu_speed/trace_quarter_speed.txt" "simulations/cpu_speed/vector_table.txt" "simulations/cpu_speed/output/execution_quarter.txt"
./interrupts "simulations/cpu_speed/trace_eigth_speed.txt" "simulations/cpu_speed/vector_table.txt" "simulations/cpu_speed/output/execution_eigth.txt"