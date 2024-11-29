#!/bin/bash
cd ./Assignment3/tests/t7
# test_scenario7.sh - Burst Patterns
cat > input_data.txt << EOF
1, 8, 0, 30, 15, 3
2, 5, 2, 20, 5, 2
3, 12, 4, 40, 10, 4
4, 3, 6, 15, 3, 1
EOF
printf '\n---------------------------------------------\nPriority Scheduler\n-----------------------------------------------------------\n'
./../../output/priority input_data.txt
printf '\n---------------------------------------------\nFCFS Scheduler\n-----------------------------------------------------------\n'
./../../output/fcfs input_data.txt
printf '\n---------------------------------------------\nRR Scheduler\n-----------------------------------------------------------\n'
./../../output/rr input_data.txt