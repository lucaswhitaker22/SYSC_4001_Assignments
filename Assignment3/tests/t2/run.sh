#!/bin/bash
cd ./Assignment3/tests/t2
# test_scenario2.sh - CPU-bound processes
cat > input_data.txt << EOF
0,8,0,100,20,5
1,12,10,80,40,10
2,6,20,60,30,5
EOF
./../../output/interrupts input_data.txt