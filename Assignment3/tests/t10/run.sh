#!/bin/bash
cd ./Assignment3/tests/t10
# test_scenario1.sh - short running processes
cat > input_data.txt << EOF
1,10,0,100,20,3
2,15,20,80,16,4
3,12,40,120,24,3
4,8,60,90,18,4
5,18,80,110,22,3
EOF
ls
./../../output/interrupts input_data.txt