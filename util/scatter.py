#!/usr/bin/env python3
import matplotlib.pyplot as plt
from pathlib import Path

enhanced = Path('enhanced.out').read_text()
elines = [ line for line in enhanced.replace(',', '').splitlines() if line.startswith('\t') ]
ex, eocc, ey = zip(*[[ int(x) for x in l.split() if x.isdigit()] for l in elines ])

zuffix = Path('zuffix.out').read_text()
zlines = [ line for line in zuffix.replace(',', '').splitlines() if line.startswith('\t') ]
zx, zocc, zy = zip(*[[ int(x) for x in l.split() if x.isdigit()] for l in zlines ])

plt.scatter(ex, ey, marker=r"$e$", edgecolors='none')
for i, lbl in enumerate(eocc): plt.annotate(lbl, (ex[i], ey[i]))

plt.scatter(zx, zy, marker=r"$z$", edgecolors='none')
for i, lbl in enumerate(zocc): plt.annotate(lbl, (zx[i], zy[i]))

plt.xscale('log', base=10)
plt.yscale('log', base=10)

plt.xlabel('Lunghezza pattern [Bytes = Caratteri]')
plt.ylabel('Tempo [ns]')

plt.show()
