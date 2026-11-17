printf '1\n' > tests/input.txt
chmod +x tests/run.sh
./esinos < tests/input.txt
cp tests/out.txt tests/expected.txt
git add tests/input.txt tests/expected.txt
make test
