#!/bin/bash

# make clean && DEBUG=0 CXX=/usr/bin/g++ make test

for file in ./benchmarks/*.hex
do
    filename=$(basename -- "$file")
    ./tinyrv -sg $file > "./logs/$filename.log"
    echo $filename $(tail -n 1 "./logs/$filename.log")
done