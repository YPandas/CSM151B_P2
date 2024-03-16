#!/bin/bash

make clean && DEBUG=0 CXX=/usr/bin/g++ make test

for file in ./benchmarks/*.hex
do
    filename=$(basename -- "$file")
    filename="${filename%.*}"
    echo "-----" $filename "-----"
    ./execute.sh $filename
done