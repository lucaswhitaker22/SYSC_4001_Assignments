#!/bin/bash
cd ./Assignment3/tests/t4
# test_scenario4.sh -Many short processes
cat > input_data.txt << EOF
1, 35, 0, 20, 5, 2
2, 5, 0, 15, 5, 2
3, 25, 0, 10, 5, 2
4, 10, 0, 25, 5, 2
5, 15, 0, 30, 5, 2
EOF
printf '\n---------------------------------------------\nPriority Scheduler\n-----------------------------------------------------------\n'
./../../output/priority input_data.txt
printf '\n---------------------------------------------\nFCFS Scheduler\n-----------------------------------------------------------\n'
./../../output/fcfs input_data.txt
printf '\n---------------------------------------------\nRR Scheduler\n-----------------------------------------------------------\n'
./../../output/rr input_data.txt