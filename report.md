Report: Improving Hash Algorithm
===

*All benchmarking is done at lnxsrv15.seas.ucla.edu for a normal student user*

## Algorithm Description

For the hash alogorithm, the CRC hash algorithm is used to generate the hash. In
this scenario CRC treats the BHR and PC_INDEX as two separate bytes. This change
of hash algorithm results in an observable increase in accuracy for most of the 
tasks provided in the benchmark resource at a slightly higher performance cost.

### CRC Hash

In CS 32, prof. Smallberg has mentioned a hash algorithm called CRC. Due to the
simplicity, it has become my first thought. After some tweaking, it does the job.

The CRC hash works by iteratively computing the XOR result of the currenct hash
with the incoming byte, then mapping it using a polynomial function to obtain 
the hash for next iteration. The pseudocode below demonstrates how a regular 
CRC works:

```
function CRC(bytes, polynomial):
    let hash = 0
    loop for byte in bytes:
        hash := polynomial(byte XOR hash)
    return hash
```

After many trial and errors, the polynomials used in the [open source library](https://www.libcrc.org/) 
`libcrc` by Lammert Bies shows the best results overall. According to the 
original source code, the data comes from a humidity sensor and does not come
with any explaination. Therefore, I was unable to find the polynomial behind.

After adopting the polynomial data, the CRC hash function is implemented 
successfully.

## Prediction Accuracy Comparison:

A benchmark is done for all 8 provided tasks, but due to the page limit, the 
following one are chosen to be shown:

- **qsort**: highest improvement in accuracy and highest decrase in performance.
- **towers**: lowest overall accuracy.
- **spmv**: largest amount of work.

|Task | Accuracy | Performance Cost |
|---|---|---|
| qsort | ![qsort accuracy](./qsort.total.log.png) | ![qsort timing](./qsort.total.log.timing.png) |
| spmv | ![spmv accuracy](./spmv.total.log.png) | ![spmv timing](./spmv.total.log.timing.png) |
| towers | ![towers accuracy](./towers.total.log.png) | ![towers timing](./towers.total.log.timing.png) |


<!-- | dhrystone | ![dhrystone accuracy](./dhrystone.total.log.png) | ![dhrystone timing](./dhrystone.total.log.timing.png) |
| median | ![median accuracy](./median.total.log.png) | ![median timing](./median.total.log.timing.png) |
| memcpy | ![memcpy accuracy](./memcpy.total.log.png) | ![memcpy timing](./memcpy.total.log.timing.png) |
| multiply | ![multiply accuracy](./multiply.total.log.png) | ![multiply timing](./multiply.total.log.timing.png) | -->
<!-- | rsort | ![rsort accuracy](./rsort.total.log.png) | ![rsort timing](./rsort.total.log.timing.png) | -->

## Results Summary

From the above figure, we can observe that there's an 1-5% improvement in 
accuracy for all cases except the rsort task; besides, we can observe a 2-12% 
decrease of performance.

Considering the scenario here, performance is critical than avoiding collisions,
and a small decrease of performance results in a massive performance loss due to
the amount of tasks a processor needs to handle. 

In CRC, the results drastically depend on the polynomials choosen. Currently the
best one is from `libcrc`, but if later another polynomial can be discovered, it
might produce a much better result than the current selected one.

In conclusion, the decrase of performance introduced by CRC may not be able to 
compensate the performance increase from improving accuracy, therefore causing 
XOR still be a very optimal solution.

## Additional Notes: Experiment Setup

**Setup**

To ease the experiment process, `gshare.cpp` is changed to allow a customized
hash function and implemented the function in a separate `hashlib.h`. For 
simplicity, a macro is used in `hashlib.h` to specify which hash function is 
being used.

The reason not to implement in a `cpp` file is to keep the `Makefile` the same
as we're provided with for easier verification.

**Testing**

For testing and benchmarking, `core.cpp` is modified to provide more data along
the execution. Besides, a Python test script is used to plot the result.

In `core.cpp`, the name of the hash algorithm, correct predictions, and total
predictions are printed in the output; then, this output is concatenated with
the output to compare and then fed into the Python script. The script plots all
data at the same figure to directly compare the results.

To further automate the process, shell scripts are used to automatically run the
program against all benchmarks provided in Project 2. 

In order to measure the performance, `gshare.cpp` is modified to record the time
for performing hash operation using the standard `chrono` library (C++ 11). 

Notice here a processing technique is used to flatten the peak that exceeds
the previous data over by 1024 (an arbitrary big number) to improve robustness. 
This is treated exactly the same for both XOR timing data and CRC timing data so
we can expect this not to introduce unfairness.


