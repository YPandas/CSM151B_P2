#!/bin/bash
# make clean && DEBUG=0 CXX=/usr/bin/g++ make test
./tinyrv -sg benchmarks/$1.hex > $1.log
tail -n 1 $1.log
tail -n 1 xor-logs/$1.hex.log
cat xor-logs/$1.hex.log $1.log > $1.total.log
python3 plot.py $1.total.log $1
python3 plot_timing.py $1.total.log $1