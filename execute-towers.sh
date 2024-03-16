make clean && DEBUG=0 CXX=/usr/bin/g++ make test
./tinyrv -sg benchmarks/towers.hex >towers.log
tail -n 1 towers.log
tail -n 1 xor-logs/towers.hex.log
cat towers.log xor-logs/towers.hex.log > towers.total.log
python3 plot.py towers.total.log Towers
python3 plot_timing.py towers.total.log Towers