#!/bin/sh
# tests/run.sh - build, run, compare against CM4 numbers
make clean && make
./esinos < tests/input.txt > tests/out.txt
diff tests/out.txt tests/expected.txt && echo "WEEK 1 GREEN"