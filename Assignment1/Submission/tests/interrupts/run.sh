#!/bin/bash

cd ./Assignment1/Submission/ || exit 1

./interrupts "tests/interrupts/trace1.txt" "tests/interrupts/vector_table.txt" "tests/interrupts/output/execution_1.txt" 
./interrupts "tests/interrupts/trace2.txt" "tests/interrupts/vector_table.txt" "tests/interrupts/output/execution_2.txt" 
./interrupts "tests/interrupts/trace3.txt" "tests/interrupts/vector_table.txt" "tests/interrupts/output/execution_3.txt" 
./interrupts "tests/interrupts/trace4.txt" "tests/interrupts/vector_table.txt" "tests/interrupts/output/execution_4.txt" 
