#!/usr/bin/env bash
cd "$(dirname "$0")"
# test_scenario10.sh - Processes with low I/O frequency
cat > input_data.txt << EOF
1, 10, 0, 40, 10, 2
2, 5, 5, 20, 5, 1
3, 15, 10, 60, 15, 3
4, 8, 15, 30, 8, 2
5, 12, 20, 50, 12, 4
EOF
printf '\n---------------------------------------------\nPriority Scheduler\n-----------------------------------------------------------\n'
./../../output/priority input_data.txt
printf '\n---------------------------------------------\nFCFS Scheduler\n-----------------------------------------------------------\n'
./../../output/fcfs input_data.txt
printf '\n---------------------------------------------\nRR Scheduler\n-----------------------------------------------------------\n'
./../../output/rr input_data.txt