#!/bin/bash
cd ./Assignment3/tests/t10
# test_scenario10.sh - Processes with low I/O frequency
cat > input_data.txt << EOF
1,10,0,40,15,3
2,12,1,35,18,4
3,8,2,45,20,5
EOF

./../../output/interrupts input_data.txt