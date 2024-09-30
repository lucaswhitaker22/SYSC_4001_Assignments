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

# Loop through each numbered subfolder in the simulations directory
for sim_num in 1 2 3 4 5; do
    echo "Running simulation $sim_num"
    
    # Check if trace file exists
    if [ ! -f "simulations/$sim_num/trace.txt" ]; then
        echo "Trace file not found for simulation $sim_num. Skipping."
        continue
    fi
    
    # Run the interrupt simulator
    ./interrupts "simulations/$sim_num/trace.txt" "simulations/vector_table.txt" "simulations/$sim_num/execution.txt"
    
    # Check if the simulation was successful
    if [ $? -eq 0 ]; then
        echo "Simulation $sim_num completed successfully"
    else
        echo "Simulation $sim_num failed"
    fi
    
    echo "------------------------"
done

echo "All simulations completed"