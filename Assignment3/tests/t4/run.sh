#!/bin/bash
cd ./Assignment3/tests/t4
# test_scenario1.sh - IO-bound processes
cat > input_data.txt << EOF
1,8,0,30,2,5
2,12,1,25,1,6
3,6,2,35,2,7
4,10,3,28,1,5
5,7,4,32,2,6
EOF
ls
./../../output/interrupts input_data.txt