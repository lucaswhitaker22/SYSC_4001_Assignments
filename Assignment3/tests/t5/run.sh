#!/bin/bash
cd ./Assignment3/tests/t5

# test_scenario5.sh - Long burst processes
cat > input_data.txt << EOF
0,25,0,100,0,0
1,10,5,60,15,3
2,15,10,80,0,0
3,8,15,40,10,2
4,12,20,70,20,4
EOF

./../../output/interrupts input_data.txt