#!/bin/bash
cd ./Assignment3/tests/t7
# test_scenario1.sh - IO-bound processes
cat > input_data.txt << EOF
1,2,0,40,8,1
2,3,1,35,7,2
3,1,2,45,9,1
4,2,3,38,8,2
5,3,4,42,9,1
EOF
ls
./../../output/interrupts input_data.txt