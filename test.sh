#!/bin/bash

passed=0
total=0

> tests/results.txt

for file in `ls tests/*.asm`; do
    total=$((total+2))
    name=$(basename -s .asm "$file")
    echo "TEST $name" >> tests/results.txt
    echo "    objdump_x2017:" >> tests/results.txt
    ./objdump_x2017 tests/$name.x2017 | diff - tests/$name.asm >> tests/results.txt && passed=$((passed+1)) && echo "Test '$name' (objdump) passed." && echo "        PASSED" >> tests/results.txt || echo "Test '$name' (objdump) failed; see results.txt"
    echo "    vm_x2017:" >> tests/results.txt
    ./vm_x2017 tests/$name.x2017 | diff - tests/$name.out >> tests/results.txt && passed=$((passed+1)) && echo "Test '$name' (vm) passed." && echo "        PASSED" >> tests/results.txt || echo "Test '$name' (vm) failed; see results.txt"
    echo "------------------------------------------------------------------------------" >> tests/results.txt
    echo
done

echo "------------------------------------------------------------------------------"
echo
echo "PASSED $passed/$total TESTS."
echo
