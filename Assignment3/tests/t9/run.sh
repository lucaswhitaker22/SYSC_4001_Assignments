#!/usr/bin/env bash
cd "$(dirname "$0")"
# test_scenario9.sh - Long-running with Frequent I/O
cat > input_data.txt << EOF
1, 5, 0, 200, 50, 5
2, 5, 0, 150, 40, 4
3, 5, 0, 100, 30, 3
4, 5, 0, 50, 20, 2
5, 5, 0, 25, 10, 1
EOF
printf '\n---------------------------------------------\nPriority Scheduler\n-----------------------------------------------------------\n'
./../../output/priority input_data.txt
printf '\n---------------------------------------------\nFCFS Scheduler\n-----------------------------------------------------------\n'
./../../output/fcfs input_data.txt
printf '\n---------------------------------------------\nRR Scheduler\n-----------------------------------------------------------\n'
./../../output/rr input_data.txt