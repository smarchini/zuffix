#!/usr/bin/env bash
# Usage: ./genplot.sh <date>
python ../util/findsdsl_plot.py "$1" proteins-200MB 10000 10000000
python ../util/findsdsl_plot.py "$1" sources-200MB 100000 100000000
python ../util/findsdsl_plot.py "$1" dna-200MB 10000 10000000
python ../util/findsdsl_plot.py "$1" dblp-xml-200MB 1000 10000000
python ../util/findsdsl_plot.py "$1" english-200MB 1000 10000000
python ../util/findsdsl_plot.py "$1" random-azAZ09-200MB 1000 10000000
python ../util/findsdsl_plot.py "$1" random-ACGT-200MB 10000 10000000
