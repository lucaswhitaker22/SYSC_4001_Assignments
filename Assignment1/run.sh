#!/bin/bash

# Change to the Part2 directory
cd ./Assignment1/Part2 || exit 1

# Compile the C++ program
g++ -std=c++11 interrupts.cpp -o interrupts

# Check if compilation was successful
if [ $? -ne 0 ]; then
    echo "Compilation failed. Exiting."
    exit 1
fi


# Run the interrupt simulator
./interrupts "./Part2/simulations/example/trace.txt" "./Part2/simulations/example/vector_table.txt" "./Part2/simulations/output/execution.txt"
