#!/bin/bash
cd ./Assignment3/tests/t9
# test_scenario1.sh - short running processes
cat > input_data.txt << EOF
1,5,0,10,2,1
2,8,1,12,3,1
3,6,2,8,2,1
4,7,3,11,3,1
5,4,4,9,2,1
EOF
ls
./../../output/interrupts input_data.txt