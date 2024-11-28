#!/bin/bash
cd ./Assignment3/tests/t7
# test_scenario7.sh - Burst Patterns
cat > input_data.txt << EOF
0,8,0,40,10,2
1,12,5,60,15,3
2,6,10,30,8,2
3,10,15,50,0,0
4,5,20,25,5,1
EOF

./../../output/interrupts input_data.txt