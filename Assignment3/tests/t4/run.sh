#!/bin/bash
cd ./Assignment3/tests/t4
# test_scenario4.sh -Many short processes
cat > input_data.txt << EOF
0,5,0,10,0,0
1,3,1,8,0,0
2,4,2,6,0,0
3,2,3,4,0,0
4,1,4,2,0,0
EOF

./../../output/interrupts input_data.txt