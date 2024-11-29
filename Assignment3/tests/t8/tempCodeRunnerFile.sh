#!/bin/bash
cd ./Assignment3/tests/t8
# test_scenario8.sh - Priority Testing (for Priority Scheduler)
cat > input_data.txt << EOF
1, 2, 0, 15, 5, 1
2, 8, 0, 20, 6, 2
3, 15, 0, 25, 7, 3
4, 25, 0, 30, 8, 4
5, 40, 0, 35, 9, 5
EOF
printf '\n---------------------------------------------\nPriority Scheduler\n-----------------------------------------------------------\n'
./../../output/priority input_data.txt
printf '\n---------------------------------------------\nFCFS Scheduler\n-----------------------------------------------------------\n'
./../../output/fcfs input_data.txt
printf '\n---------------------------------------------\nRR Scheduler\n-----------------------------------------------------------\n'
./../../output/rr input_data.txt