#!/bin/bash
cd ./Assignment3/tests/t4
# test_scenario4.sh - Short burst processes
cat > input_data.txt << EOF
1,3,0,10,2,1
2,4,1,8,3,1
3,2,2,12,4,2
4,5,3,9,3,1
EOF

./../../output/interrupts input_data.txt