#!/usr/bin/env python3
import csv
import matplotlib.pyplot as plt
from pathlib import Path

def readcsv(filename):
    with open(filename) as fp:
        reader = csv.reader(fp, delimiter=",", quotechar='"')
        next(reader, None)  # skip the headers
        return [row for row in reader]

alldata = readcsv('../results/hash_block_size.csv')

for name in [ 'BM_XXH3', 'BM_RabinKarp', 'BM_CyclicPoly128' ]:
    data = [ line for line in alldata if line[0].startswith(name) ]
    L = [ line[0].split('/')[0][len(name)+1:-7] for line in data ]
    X = [ int(line[0].split('/')[1]) for line in data ]
    Y = [ float(line[2]) for line in data ]
    # draw points in the figure
    fig, ax = plt.subplots(figsize=(4.5,7))
    for (l, x, y) in zip(L[:-1], X[:-1], Y[:-1]):
        ax.scatter(x, y, c='black', s=150*len(l), marker=r"${}$".format(l), edgecolors='none')
    # figure parameters
    ax.set_xscale('log', base=2)
    ax.set_yscale('linear')
    ax.set_xticks(sorted(set(X)))
    plt.xlabel('String length [B]', fontsize=12)
    plt.ylabel('Time [ns]', fontsize=12)
    plt.title(name[3:], fontsize=16)
    fig.savefig(name + '.pdf', bbox_inches='tight')
    #plt.show()
