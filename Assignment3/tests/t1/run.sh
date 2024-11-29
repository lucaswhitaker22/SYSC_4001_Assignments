#!/bin/bash
cd ./Assignment3/tests/t1
# test_scenario1.sh - IO-bound processes
cat > input_data.txt << EOF
15, 10, 0, 25, 11, 3

EOF

ls
./../../output/priority input_data.txt
./../../output/fcfs input_data.txt
./../../output/rr input_data.txt