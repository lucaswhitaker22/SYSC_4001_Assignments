#!/bin/bash
cd ./Assignment3/tests/t6
# test_scenario6.sh - High memory requirement processes
cat > input_data.txt << EOF
1,10,0,50,15,2
2,15,1,40,20,1
3,8,2,60,25,2
EOF

./../../output/interrupts input_data.txt