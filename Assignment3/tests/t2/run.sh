#!/usr/bin/env bash
cd "$(dirname "$0")"

# test_scenario2.sh - CPU-bound processes
cat > input_data.txt << EOF
15, 10, 0, 25, 11, 3
12, 1, 0, 50, 10, 5
1, 10, 0, 100, 25, 25
2, 1, 5, 20, 10, 2
3, 7, 14, 5, 2, 10
EOF
printf '\n---------------------------------------------\nPriority Scheduler\n-----------------------------------------------------------\n'
./../../output/priority input_data.txt
printf '\n---------------------------------------------\nFCFS Scheduler\n-----------------------------------------------------------\n'
./../../output/fcfs input_data.txt
printf '\n---------------------------------------------\nRR Scheduler\n-----------------------------------------------------------\n'
./../../output/rr input_data.txt