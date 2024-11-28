#!/bin/bash
cd ./Assignment3/tests/t6
# test_scenario6.sh -  memory constraints
cat > input_data.txt << EOF
0,40,0,100,20,5
1,25,5,80,0,0
2,15,10,60,30,10
3,10,15,40,10,2
4,8,20,30,0,0
5,2,25,20,5,1
EOF

./../../output/interrupts input_data.txt