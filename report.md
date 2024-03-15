Report: Improving Hit Rate via Hashing
===

*Author: Dyson (Daixuan) Huo*

## Abstract

## Introduction

## Experiment Approaches

### Setup

To ease the experiment process, I modified `gshare.cpp` to allow a customized
hash function and implemented the function in a separate `hashlib.h`.

The reason not to implement in a `cpp` file is to keep the `Makefile` the same
as we're provided with for easier verification.

### Testing

For testing and benchmarking, `core.cpp` is modified to provide more data along
the execution. Besides, a Python test script is used to plot the result.

In `core.cpp`, the name of the hash algorithm, correct predictions, and total
predictions are printed in the output; then, this output is concatenated with
the output to compare and then fed into the Python script. The script plots all
data at the same figure to directly compare the results.

To further automate the process, a shell script is used to automatically run the
program against all benchmarks provided in Project 2. 

In order to measure the performance, `gshare.cpp` is modified to record the time
for performing hash operation using the standard `chrono` library (C++ 11).



## Results & Analysis

## Conclusion


