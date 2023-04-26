#!/usr/bin/env bash
# Usage: ./genplot.sh <date>
python ../util/findrandom_plot.py "$1" proteins-200MB 15000
python ../util/findrandom_plot.py "$1" sources-200MB 20000
python ../util/findrandom_plot.py "$1" dna-200MB 6000
python ../util/findrandom_plot.py "$1" dblp-xml-200MB 6000
python ../util/findrandom_plot.py "$1" english-200MB 16000
python ../util/findrandom_plot.py "$1" random-azAZ09-200MB 5500
python ../util/findrandom_plot.py "$1" random-ACGT-200MB 3500
python ../util/fibonacci_plot.py "$1"
