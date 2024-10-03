#!/bin/bash

# Change to the Part2 directory
cd ./Assignment1/Part2

./interrupts "simulations/cpu_io/trace1.txt" "simulations/cpu_io/vector_table.txt" "simulations/cpu_io/output/execution1.txt" 
./interrupts "simulations/cpu_io/trace2.txt" "simulations/cpu_io/vector_table.txt" "simulations/cpu_io/output/execution2.txt" 
./interrupts "simulations/cpu_io/trace3.txt" "simulations/cpu_io/vector_table.txt" "simulations/cpu_io/output/execution3.txt"
./interrupts "simulations/cpu_io/trace4.txt" "simulations/cpu_io/vector_table.txt" "simulations/cpu_io/output/execution4.txt"
