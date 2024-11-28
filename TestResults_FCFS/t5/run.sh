#!/bin/bash
cd ./Assignment3/tests/t4

# test_scenario5.sh - Long burst processes
cat > input_data.txt << EOF
1,20,0,100,30,5
2,25,2,80,25,4
3,15,4,120,35,6
EOF

./../../output/interrupts input_data.txt