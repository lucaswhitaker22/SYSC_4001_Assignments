#!/bin/bash
cd ./Assignment3/tests/t1
# test_scenario1.sh - IO-bound processes
cat > input_data.txt << EOF
1,5,0,20,5,3
2,8,0,15,3,4
3,10,2,25,4,5
4,4,3,30,6,4
5,6,4,18,3,3
EOF

ls
./../../output/interrupts input_data.txt