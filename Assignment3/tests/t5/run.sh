#!/usr/bin/env bash
cd "$(dirname "$0")"


# test_scenario5.sh - Long burst processes
cat > input_data.txt << EOF
1, 10, 0, 20, 5, 2
2, 10, 5, 15, 5, 2
3, 10, 10, 10, 5, 2
4, 10, 15, 25, 5, 2
EOF

printf '\n---------------------------------------------\nPriority Scheduler\n-----------------------------------------------------------\n'
./../../output/priority input_data.txt
printf '\n---------------------------------------------\nFCFS Scheduler\n-----------------------------------------------------------\n'
./../../output/fcfs input_data.txt
printf '\n---------------------------------------------\nRR Scheduler\n-----------------------------------------------------------\n'
./../../output/rr input_data.txt