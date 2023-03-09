#!/usr/bin/env python3
import pandas as pd
import matplotlib.pyplot as plt

files = [ '/home/pacman616/src/zuffix/simple.csv',
          '/home/pacman616/src/zuffix/enhanced.csv',
          '/home/pacman616/src/zuffix/exact-zuffix-xxh3.csv',
          '/home/pacman616/src/zuffix/probabilistic-zuffix-xxh3.csv']

fig = plt.figure(figsize=(9.5,7))
ax = fig.add_subplot(1,1,1)

for file in files:
    print(f"file => {file}")
    table = pd.read_csv(file)
    x = [ value.split('/')[1] for value in table['name'] ]
    y = list(table['cpu_time'])
    lbl = file.split('/')[-1][:-4]
    print(lbl)
    ax.semilogy(x, y, label=lbl)

plt.legend(loc="upper left", bbox_to_anchor=[0, 1], ncol=2, shadow=True, fancybox=True)
plt.title("Random: $|T| = 50MB$, $\sigma = 4$, patterns extracted from T", fontsize='xx-large')
plt.xlabel("Pattern length (Bytes)", fontsize='large')
plt.ylabel("Time (ns)", fontsize='large')

fig.show()
