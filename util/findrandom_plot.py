#!/usr/bin/env python3
import pandas as pd
import matplotlib.pyplot as plt

def to_si(n):
    si = [ ('T', 1e12), ('G', 1e9), ('M', 1e6), ('k', 1e3), ('', 1e0) ]
    for (sym, base) in si:
        if base <= n:
            return f'{n/base:.2f}'.rstrip('0').rstrip('.') + sym

title = 'gutenberg-200MB'
date = '20230313'

benchmarks = [
    (f'~/blew_results/findrandom.{title}.simple.{date}.csv', '-', 'grey'),
    (f'~/blew_results/findrandom.{title}.enhanced.{date}.csv', '-', 'black'),
    (f'~/blew_results/findrandom.{title}.memcmp-zuffix-wyhash.{date}.csv', '-', 'red'),
    (f'~/blew_results/findrandom.{title}.memcmp-zuffix-xxh3.{date}.csv', '-', 'purple'),
    (f'~/blew_results/findrandom.{title}.memcmp-zuffix-crc32cfolly.{date}.csv', '-', 'blue'),
    (f'~/blew_results/findrandom.{title}.memcmp-zuffix-crc32zlib.{date}.csv', '-', 'green'),
    (f'~/blew_results/findrandom.{title}.memcmp-zuffix-crc32+crc32c.{date}.csv', '-', 'brown'),
    (f'~/blew_results/findrandom.{title}.signature-zuffix-wyhash.{date}.csv', '-.', 'red'),
    (f'~/blew_results/findrandom.{title}.signature-zuffix-crc32cfolly.{date}.csv', '-.', 'blue'),
    (f'~/blew_results/findrandom.{title}.signature-zuffix-crc32zlib.{date}.csv', '-.', 'green'),
    (f'~/blew_results/findrandom.{title}.signature-zuffix-crc32+crc32c.{date}.csv', '-.', 'brown'),
    (f'~/blew_results/findrandom.{title}.nothing-zuffix-wyhash.{date}.csv', ':', 'red'),
    (f'~/blew_results/findrandom.{title}.nothing-zuffix-crc32cfolly.{date}.csv', ':', 'blue'),
]

fig = plt.figure(figsize=(16,9))
ax = fig.add_subplot(1,1,1)

locmin = matplotlib.ticker.LogLocator(base=10.0, subs=(0.1,0.2,0.4,0.6,0.8,1,2,4,6,8,10 ))
ax.xaxis.set_minor_locator(locmin)
ax.xaxis.set_minor_formatter(matplotlib.ticker.NullFormatter())

for (file, line, color) in benchmarks:
    name = file.split('/')[-1].split('.')[2]
    table = pd.read_csv(file)
    x = [ int(value.split('/')[1]) for value in table['name'] ]
    y = list(table['cpu_time'])
    lbl = list(table['errors'])
    iterations = int(table['iterations'][0])
    ax.loglog(x, y, label=name, linestyle=line, color=color)
    if name == 'simple':
        topax = ax.secondary_xaxis('top')
        topax.tick_params(axis='x', direction='inout')
        topax.set_ticks(x, [ to_si(int(o)) for o in table['occurrences'] ], ha='left', rotation=45)
        topax.set_xlabel('Occurrences')
    for (xval, yval, lblval) in zip(x, y, lbl):
        if lblval != 0: ax.annotate(lblval, xy=(xval, yval), ha='left', rotation=60)

plt.xlim([1, 100 * (1 << 20)])
ax.xaxis.get_major_locator().set_params(numticks=99)
ax.xaxis.get_minor_locator().set_params(numticks=99, subs=[.1, .2, .3, .4, .5, .6, .7, .8, .9])
plt.legend(loc="lower right", ncols=2)
plt.title(f'{title} (10000 iterations)', fontsize='xx-large')
plt.xlabel("Pattern length (Bytes)", fontsize='large')
plt.ylabel("Time (ns)", fontsize='large')

plt.savefig(f'{title}.{date}.png', dpi=1200)
#fig.show()
