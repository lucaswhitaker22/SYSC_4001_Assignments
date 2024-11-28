#!/bin/bash
cd ./Assignment3/tests/t2
# test_scenario2.sh - CPU-bound processes
cat > input_data.txt << EOF
1,10,0,50,15,2
2,15,1,40,20,1
3,8,2,60,25,2
EOF
./../../output/interrupts input_data.txt