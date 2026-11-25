#!/bin/sh
cd "$(dirname "$0")/.." || exit 1
[ -f tests/input.txt ] || printf '1\n' > tests/input.txt
./esinos < tests/input.txt > tests/out.txt 2>&1
if diff -u tests/expected.txt tests/out.txt > tests/diff.txt 2>&1; then
    echo "WEEK 1 SCHED GREEN"
    rm -f tests/diff.txt
    exit 0
fi
echo "WEEK 1 SCHED RED : diff dans tests/diff.txt"
exit 1
