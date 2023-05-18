#!/usr/bin/env bash
# Usage: ./genplot.sh <date>
# python ../util/findrandom_plot.py "$1" proteins-200MB
# python ../util/findrandom_plot.py "$1" sources-200MB
# python ../util/findrandom_plot.py "$1" dna-200MB
# python ../util/findrandom_plot.py "$1" dblp-xml-200MB
# python ../util/findrandom_plot.py "$1" english-200MB
# python ../util/findrandom_plot.py "$1" random-azAZ09-200MB
# python ../util/findrandom_plot.py "$1" random-ACGT-200MB
# python ../util/fibonacci_plot.py "$1"


#!/usr/bin/env bash
# Usage: ./genplot.sh <date>
python ../util/findrandom_plot.py "$1" proteins-200MB 100000000
python ../util/findrandom_plot.py "$1" sources-200MB  100000000
python ../util/findrandom_plot.py "$1" dna-200MB  100000000
python ../util/findrandom_plot.py "$1" dblp-xml-200MB  100000000
python ../util/findrandom_plot.py "$1" english-200MB  100000000
python ../util/findrandom_plot.py "$1" random-azAZ09-200MB  100000000
python ../util/findrandom_plot.py "$1" random-ACGT-200MB  100000000
python ../util/fibonacci_plot.py "$1"
