#!/usr/bin/env python3
import sys
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
title = sys.argv[2]
xlim = int(sys.argv[3]) if len(sys.argv) > 3 else None
ylim = int(sys.argv[4]) if len(sys.argv) > 4 else None
prefixdir = '.'

benchmarks = [
    #(f'{prefixdir}/{date}/findsdsl.{title}.sct3-backward.{date}.csv', '-', 'grey'),
    (f'{prefixdir}/{date}/findsdsl.{title}.sct3-forward.{date}.csv', '-', 'black'),
    (f'{prefixdir}/{date}/findsdsl.{title}.sct3-zfast-forward-wyhash.{date}.csv', '-', 'red'),
    (f'{prefixdir}/{date}/findsdsl.{title}.sct3-zfast-forward-xxh3.{date}.csv', '-', 'purple'),
    #(f'{prefixdir}/{date}/findsdsl.{title}.sct3-zfast-forward-crc32cfolly.{date}.csv', '-', 'blue'),
    #(f'{prefixdir}/{date}/findsdsl.{title}.sct3-zfast-forward-crc32zlib.{date}.csv', '-', 'green'),
    (f'{prefixdir}/{date}/findsdsl.{title}.sct3-zfast-forward-crc32+crc32c.{date}.csv', '-', 'brown'),
    #(f'{prefixdir}/{date}/findsdsl.{title}.signature-zuffix-crc32cfolly.{date}.csv', '-.', 'blue'),
    #(f'{prefixdir}/{date}/findsdsl.{title}.signature-zuffix-crc32zlib.{date}.csv', '-.', 'green'),
    #(f'{prefixdir}/{date}/findsdsl.{title}.signature-zuffix-crc32+crc32c.{date}.csv', '-.', 'brown'),
]

fig = plt.figure(figsize=(16,9))
ax = fig.add_subplot(1,1,1)

locmin = matplotlib.ticker.LogLocator(base=10.0, subs=(0.1,0.2,0.4,0.6,0.8,1,2,4,6,8,10 ))
ax.xaxis.set_minor_locator(locmin)
ax.xaxis.set_minor_formatter(matplotlib.ticker.NullFormatter())

for (file, line, color) in benchmarks:
    if not os.path.exists(file): continue
    name = file.split('/')[-1].split('.')[2]
    table = pd.read_csv(file, skiprows=1)
    x = table['length']
    y = list(table['cpu_time'])
    lbl = list(table['errors'])
    iterations = int(table['iterations'][0])
    ax.loglog(x, y, label=name, linestyle=line, color=color)
    #if not ylim: ax.loglog(x, y, label=name, linestyle=line, color=color)
    #else: ax.semilogx(x, y, label=name, linestyle=line, color=color)
    if name == 'enhanced':
        topax = ax.secondary_xaxis('top')
        topax.tick_params(axis='x', direction='inout')
        avg_occurrences = map(lambda x: x[0]/x[1], zip(table['occurrences'], table['iterations']))
        topax.set_ticks(x, [ to_si(o) for o in avg_occurrences ], ha='left', rotation=45)
        topax.set_xlabel('Average occurrences per iteration')
    for (xval, yval, lblval) in zip(x, y, lbl):
        if lblval != 0: ax.annotate(lblval, xy=(xval, yval), ha='left', rotation=60)

if xlim: plt.xlim([1, xlim])
if ylim: plt.ylim([100, ylim])
ax.xaxis.get_major_locator().set_params(numticks=99)
ax.xaxis.get_minor_locator().set_params(numticks=99, subs=[.1, .2, .3, .4, .5, .6, .7, .8, .9])
plt.legend(loc="lower right", ncols=2)
plt.title(f'{title}', fontsize='xx-large')
plt.xlabel("Pattern length (Bytes)", fontsize='large')
plt.ylabel("Time (ns)", fontsize='large')

plt.savefig(f'{title}.{date}.pdf')
#fig.show()
