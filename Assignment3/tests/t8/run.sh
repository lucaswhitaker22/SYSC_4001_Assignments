#!/bin/bash
cd ./Assignment3/tests/t8
# test_scenario8.sh - Priority Testing (for Priority Scheduler)
cat > input_data.txt << EOF
0,20,0,100,0,0,3
1,15,5,80,20,5,1
2,10,10,60,0,0,2
3,25,15,120,30,10,4
4,5,20,40,10,2,5
EOF

./../../output/interrupts input_data.txt