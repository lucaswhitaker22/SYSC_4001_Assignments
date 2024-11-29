#!/bin/bash
cd ./Assignment3/tests/t1
# test_scenario1.sh - IO-bound processes
cat > input_data.txt << EOF
15, 10, 0, 25, 11, 3

EOF
printf '\n---------------------------------------------\nPriority Scheduler\n-----------------------------------------------------------\n'
./../../output/priority input_data.txt
printf '\n---------------------------------------------\nFCFS Scheduler\n-----------------------------------------------------------\n'
./../../output/fcfs input_data.txt
printf '\n---------------------------------------------\nRR Scheduler\n-----------------------------------------------------------\n'
./../../output/rr input_data.txt