#!/bin/bash
compilerExecutable=tigerc
dirOfTestCases=./test-cases/

for testFile in $(ls $dirOfTestCases)
do
    ./$compilerExecutable $dirOfTestCases$testFile &>/dev/null
    if [ $? -eq 0 ]; then
        echo "Test success for " $testFile " exit code: " $?
    else
        echo "Test fail for " $testFile " exit code: " $?
    fi
done