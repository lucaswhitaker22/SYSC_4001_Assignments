#!/bin/bash
cd ./Assignment3/tests/t9
# test_scenario9.sh - Processes with high I/O frequency
cat > input_data.txt << EOF
1,5,0,30,3,2
2,8,1,25,2,1
3,6,2,35,4,2
4,7,3,28,3,1
EOF

./../../output/interrupts input_data.txt