#!/bin/bash
cd ./Assignment3/tests/t1
# test_scenario1.sh - IO-bound processes
cat > input_data.txt << EOF
1,5,0,20,5,3
2,8,0,15,3,4
EOF

ls
./../../output/interrupts input_data.txt