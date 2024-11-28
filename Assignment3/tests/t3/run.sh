#!/bin/bash
cd ./Assignment3/tests/t3
# test_scenario1.sh - IO-bound processes
cat > input_data.txt << EOF
1,20,0,200,100,1
2,25,1,180,90,2
3,15,2,220,110,1
4,30,3,190,95,2
5,18,4,210,105,1
EOF
ls
./../../output/interrupts input_data.txt