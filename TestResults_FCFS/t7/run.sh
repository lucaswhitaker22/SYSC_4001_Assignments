#!/bin/bash
cd ./Assignment3/tests/t7
# test_scenario7.sh - Low memory requirement processes
cat > input_data.txt << EOF
1,2,0,15,4,1
2,3,1,18,5,2
3,4,2,20,6,2
EOF

./../../output/interrupts input_data.txt