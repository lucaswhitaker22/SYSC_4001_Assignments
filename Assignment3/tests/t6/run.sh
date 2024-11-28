#!/bin/bash
cd ./Assignment3/tests/t6
# test_scenario1.sh - IO-bound processes
cat > input_data.txt << EOF
1,35,0,80,10,2
2,28,1,90,15,3
3,38,2,70,12,2
4,30,3,85,14,3
5,32,4,75,11,2
EOF
ls
./../../output/interrupts input_data.txt