import sys
import matplotlib.pyplot as plt

if len(sys.argv) < 2:
    exit(1)

lines = []
with open(sys.argv[1], 'r') as fr:
    lines = fr.readlines()
    
X = {}
Y = {}
for line in lines:
    if 'BENCHMARK#' in line:
        name, correct, total = line.replace('BENCHMARK#','').split(',')
        correct = int(correct); total = int(total)
        if name in X:
            X[name].append(total)
        else:
            X[name] = [total]
        if name in Y:
            Y[name].append(correct*1e2/total)
        else:
            Y[name] = [correct*1e2/total]

plt.title(f'Accuracy over predictions on task: {sys.argv[2]}')
plt.xlabel('predictions')
plt.ylabel('Accuracy (%)')

for name, y in Y.items():
    plt.plot(X[name],y, label=name)
plt.legend()
plt.savefig(sys.argv[1]+'.png')