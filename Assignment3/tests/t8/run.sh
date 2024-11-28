#!/bin/bash
cd ./Assignment3/tests/t8
# test_scenario8.sh - Processes with varying arrival times
cat > input_data.txt << EOF
1,8,0,25,7,2
2,10,5,30,8,3
3,6,10,20,6,2
EOF

./../../output/interrupts input_data.txt