#!/bin/bash

cd ./Assignment1/Submission/ || exit 1

./interrupts "tests/context_switch/trace.txt" "tests/context_switch/vector_table.txt" "tests/context_switch/output/execution_1_3.txt" 1 3
./interrupts "tests/context_switch/trace.txt" "tests/context_switch/vector_table.txt" "tests/context_switch/output/execution_3_5.txt" 3 5
./interrupts "tests/context_switch/trace.txt" "tests/context_switch/vector_table.txt" "tests/context_switch/output/execution_5_10.txt" 5 10
./interrupts "tests/context_switch/trace.txt" "tests/context_switch/vector_table.txt" "tests/context_switch/output/execution_10_20.txt" 10 20

