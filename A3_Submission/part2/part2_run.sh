#!/usr/bin/env bash
cd "$(dirname "${BASH_SOURCE[0]}")" || exit

gcc -o part2 part2_101195445_101244916.c -pthread
./part2