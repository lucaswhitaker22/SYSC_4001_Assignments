#!/bin/bash
cd ./Assignment3/tests/t3
# test_scenario3.sh - Mixed CPU and I/O bound processes
cat > input_data.txt << EOF
1,7,0,30,8,3
2,12,1,25,12,2
3,5,2,40,6,4
4,9,3,35,15,1
EOF

./../../output/interrupts input_data.txt