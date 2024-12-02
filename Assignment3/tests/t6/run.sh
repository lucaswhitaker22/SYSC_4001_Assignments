#!/usr/bin/env bash
cd "$(dirname "$0")"

# test_scenario6.sh -  memory constraints
cat > input_data.txt << EOF
1, 5, 0, 50, 2, 5
2, 5, 0, 40, 2, 4
3, 5, 0, 30, 2, 3
4, 5, 0, 20, 2, 2
5, 5, 0, 10, 2, 1
EOF
printf '\n---------------------------------------------\nPriority Scheduler\n-----------------------------------------------------------\n'
./../../output/priority input_data.txt
printf '\n---------------------------------------------\nFCFS Scheduler\n-----------------------------------------------------------\n'
./../../output/fcfs input_data.txt
printf '\n---------------------------------------------\nRR Scheduler\n-----------------------------------------------------------\n'
./../../output/rr input_data.txt