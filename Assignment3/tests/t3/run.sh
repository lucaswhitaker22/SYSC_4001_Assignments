#!/bin/bash
cd ./Assignment3/tests/t3
# test_scenario3.sh - Mixed CPU and I/O bound processes
cat > input_data.txt << EOF
0,20,0,500,100,10
1,15,50,300,0,0
2,10,100,200,50,5
EOF

./../../output/interrupts input_data.txt