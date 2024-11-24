#!/bin/bash
cd ./Assignment3/tests/t2
# test_scenario1.sh - IO-bound processes
cat > input_data_cpu.txt << EOF
1,10,0,100,25,2
2,15,0,80,30,1
3,8,2,120,40,2
4,12,3,90,35,1
5,6,4,110,45,2
EOF
ls
./../../output/interrupts input_data_cpu.txt