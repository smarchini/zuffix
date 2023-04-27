#!/usr/bin/env bash
# Usage: ./genplot.sh <date>
python ../util/findrandom_plot.py "$1" proteins-200MB 100000 13000
python ../util/findrandom_plot.py "$1" sources-200MB 100000 20000
python ../util/findrandom_plot.py "$1" dna-200MB 10000 8000
python ../util/findrandom_plot.py "$1" dblp-xml-200MB 1000 6000
python ../util/findrandom_plot.py "$1" english-200MB 100000 14000
python ../util/findrandom_plot.py "$1" random-azAZ09-200MB 10000 5500
python ../util/findrandom_plot.py "$1" random-ACGT-200MB 10000 3500
python ../util/fibonacci_plot.py "$1"
