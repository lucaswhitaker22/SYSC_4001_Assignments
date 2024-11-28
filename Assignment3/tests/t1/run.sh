#!/bin/bash
cd ./Assignment3/tests/t1
# test_scenario1.sh - IO-bound processes
cat > input_data.txt << EOF
0,10,0,50,0,0
1,15,0,30,0,0
2,5,0,20,0,0
EOF

ls
./../../output/interrupts input_data.txt