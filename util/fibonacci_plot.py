#!/usr/bin/env python3
import sys
import math
import os.path
import pandas as pd
import matplotlib
import matplotlib.pyplot as plt

def to_si(n):
    si = [ ('T', 1e12), ('G', 1e9), ('M', 1e6), ('k', 1e3), ('', 1e0) ]
    for (sym, base) in si:
        if base <= n:
            return f'{n/base:.2f}'.rstrip('0').rstrip('.') + sym

date = sys.argv[1]
prefixdir = '.'

benchmarks = [
    (f'{prefixdir}/{date}/fibonacci.simple.{date}.csv', '-', 'grey'),
    (f'{prefixdir}/{date}/fibonacci.enhanced.{date}.csv', '-', 'black'),
    (f'{prefixdir}/{date}/fibonacci.memcmp-zuffix-wyhash.{date}.csv', '-', 'red'),
    (f'{prefixdir}/{date}/fibonacci.memcmp-zuffix-xxh3.{date}.csv', '-', 'purple'),
    (f'{prefixdir}/{date}/fibonacci.memcmp-zuffix-crc32cfolly.{date}.csv', '-', 'blue'),
    (f'{prefixdir}/{date}/fibonacci.memcmp-zuffix-crc32zlib.{date}.csv', '-', 'green'),
    (f'{prefixdir}/{date}/fibonacci.memcmp-zuffix-crc32+crc32c.{date}.csv', '-', 'brown'),
    (f'{prefixdir}/{date}/fibonacci.signature-zuffix-crc32cfolly.{date}.csv', '-.', 'blue'),
    (f'{prefixdir}/{date}/fibonacci.signature-zuffix-crc32zlib.{date}.csv', '-.', 'green'),
    (f'{prefixdir}/{date}/fibonacci.signature-zuffix-crc32+crc32c.{date}.csv', '-.', 'brown'),
    (f'{prefixdir}/{date}/fibonacci.nothing-zuffix-wyhash.{date}.csv', ':', 'red'),
    (f'{prefixdir}/{date}/fibonacci.nothing-zuffix-xxh3.{date}.csv', ':', 'purple'),
    (f'{prefixdir}/{date}/fibonacci.nothing-zuffix-crc32zlib.{date}.csv', ':', 'green'),
    (f'{prefixdir}/{date}/fibonacci.nothing-zuffix-crc32+crc32c.{date}.csv', ':', 'brown'),
    (f'{prefixdir}/{date}/fibonacci.nothing-zuffix-crc32cfolly.{date}.csv', ':', 'blue'),
]

fig = plt.figure(figsize=(16,9))
ax = fig.add_subplot(1,1,1)

for (file, line, color) in benchmarks:
    if not os.path.exists(file): continue
    name = file.split('/')[-1].split('.')[1]
    table = pd.read_csv(file, skiprows=1)
    x = table['length']
    y = list(table['cpu_time'])
    lbl = list(table['errors'])
    iterations = int(table['iterations'][0])
    ax.loglog(x, y, label=name, linestyle=line, color=color)
    if name == 'enhanced':
        topax = ax.secondary_xaxis('top')
        topax.tick_params(axis='x', direction='inout')
        avg_occurrences = map(lambda x: x[0]/x[1], zip(table['occurrences'], table['iterations']))
        topax.set_ticks(x, [ to_si(int(o)) for o in avg_occurrences ], ha='left', rotation=45)
        topax.set_xlabel('Occurrences')
    for (xval, yval, lblval) in zip(x, y, lbl):
        if lblval != 0: ax.annotate(lblval, xy=(xval, yval), ha='left', rotation=60)

plt.legend(loc="lower right", ncols=2)
plt.title(f'Fibonacci strings', fontsize='xx-large')
plt.xlabel("Pattern length (Bytes)", fontsize='large')
plt.ylabel("Time (ns)", fontsize='large')

plt.savefig(f'fibonacci.{date}.pdf')
#plt.savefig(f'fibonacci.{date}.png', dpi=1200)
#fig.show()
