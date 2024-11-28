#!/bin/bash
cd ./Assignment3/tests/t8
# test_scenario1.sh - IO-bound processes
cat > input_data.txt << EOF
1,15,0,500,50,5
2,20,10,450,45,6
3,18,20,550,55,5
4,22,30,480,48,6
5,17,40,520,52,5
EOF
ls
./../../output/interrupts input_data.txt

echo "Test complete."