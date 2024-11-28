#!/bin/bash
cd ./Assignment3/tests/t5
# test_scenario1.sh - IO-bound processes
cat > input_data.txt << EOF
1,15,0,100,20,3
2,10,1,50,5,4
3,20,2,150,30,2
4,8,3,40,4,5
5,25,4,200,40,1
EOF
ls
./../../output/interrupts input_data.txt