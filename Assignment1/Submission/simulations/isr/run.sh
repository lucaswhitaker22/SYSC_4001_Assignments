#!/bin/bash


# Change to the Part2 directory
cd ./Assignment1/Part2
g++ -std=c++11 interrupts.cpp -o interrupts

./interrupts "simulations/isr/trace_1.txt" "simulations/isr/vector_table.txt" "simulations/isr/output/execution_1.txt" 
./interrupts "simulations/isr/trace_2.txt" "simulations/isr/vector_table.txt" "simulations/isr/output/execution_2.txt" 
./interrupts "simulations/isr/trace_3.txt" "simulations/isr/vector_table.txt" "simulations/isr/output/execution_3.txt" 
./interrupts "simulations/isr/trace_4.txt" "simulations/isr/vector_table.txt" "simulations/isr/output/execution_4.txt" 
./interrupts "simulations/isr/trace_5.txt" "simulations/isr/vector_table.txt" "simulations/isr/output/execution_5.txt" 
