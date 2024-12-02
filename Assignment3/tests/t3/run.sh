#!/usr/bin/env bash
cd "$(dirname "$0")"

# test_scenario3.sh - High Priority Short Jobs
cat > input_data.txt << EOF
1, 5, 0, 10, 5, 2
2, 5, 0, 8, 4, 1
3, 5, 0, 6, 3, 1
4, 5, 0, 4, 2, 1
5, 5, 0, 2, 1, 1
EOF
printf '\n---------------------------------------------\nPriority Scheduler\n-----------------------------------------------------------\n'
./../../output/priority input_data.txt
printf '\n---------------------------------------------\nFCFS Scheduler\n-----------------------------------------------------------\n'
./../../output/fcfs input_data.txt
printf '\n---------------------------------------------\nRR Scheduler\n-----------------------------------------------------------\n'
./../../output/rr input_data.txt