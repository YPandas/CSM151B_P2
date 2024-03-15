import sys
import matplotlib.pyplot as plt

if len(sys.argv) < 2:
    exit(1)

lines = []
with open(sys.argv[1], 'r') as fr:
    lines = fr.readlines()
    
X = []
Y = {}
count = 0
for line in lines:
    if 'TIMING#' in line:
        name, timing = line.replace('TIMING#','').split(',')
        timing = float(timing)
        count += 1
        X.append(count)
        if name in Y:
            Y[name].append(timing)
        else:
            Y[name] = [timing]

plt.title(f'Hash duration over predictions on task: {sys.argv[2]}')
plt.xlabel('predictions')
plt.ylabel('duration (ns)')

for name, y in Y.items():
    plt.plot(X,y, label=name)
plt.legend()
plt.savefig(sys.argv[1]+'.timing.png')