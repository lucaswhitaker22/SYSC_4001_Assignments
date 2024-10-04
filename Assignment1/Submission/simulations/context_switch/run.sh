#!/bin/bash

# Change to the Part2 directory
cd ./Assignment1/Part2

./interrupts "simulations/context_switch/trace.txt" "simulations/context_switch/vector_table.txt" "simulations/context_switch/output/execution_1_3.txt" 1 3
./interrupts "simulations/context_switch/trace.txt" "simulations/context_switch/vector_table.txt" "simulations/context_switch/output/execution_3_5.txt" 3 5
./interrupts "simulations/context_switch/trace.txt" "simulations/context_switch/vector_table.txt" "simulations/context_switch/output/execution_5_10.txt" 5 10
./interrupts "simulations/context_switch/trace.txt" "simulations/context_switch/vector_table.txt" "simulations/context_switch/output/execution_10_20.txt" 10 20

