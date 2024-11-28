#!/bin/bash
cd ./Assignment3/tests/t6
# test_scenario6.sh - High memory requirement processes
cat > input_data.txt << EOF
1,35,0,40,10,3
2,28,1,35,8,2
3,32,2,45,12,4
4,30,3,38,9,3
5,25,4,42,11,3
EOF

./../../output/interrupts input_data.txt