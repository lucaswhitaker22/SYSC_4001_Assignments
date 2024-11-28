#!/bin/bash
cd ./Assignment3/tests/t9
# test_scenario9.sh - Long-running with Frequent I/O
cat > input_data.txt << EOF
0,20,0,300,40,8
1,15,10,250,30,6
2,18,20,280,35,7
EOF

./../../output/interrupts input_data.txt