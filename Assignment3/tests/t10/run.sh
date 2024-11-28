#!/bin/bash
cd ./Assignment3/tests/t10
# test_scenario10.sh - Processes with low I/O frequency
cat > input_data.txt << EOF
0,8,0,30,0,0
1,12,5,80,20,4
2,6,10,25,0,0
3,15,15,60,15,3
4,10,20,40,10,2
EOF

./../../output/interrupts input_data.txt