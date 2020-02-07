# How to run tests and experiments

After cloning the repository it is necessary to download some external
dependencies:
- download and initialize external modules: `make external`
- download and extract the Pizza&Chili dataset: `make dataset`
- generate valid random patterns form dataset files: `make utils`
  1. automatically `make patterns`;
  2. or manually, using Pizza&Chili utils (see [experiments]) under the
     `pizzachili` directory.

The external modules consists in [sux] library for internal use, [r-index] and
[CSA++] data structures for benchmarks/tests comparison and [sdsl-lite] library
(required by r-index and CSA++).

## Tests

To build and run tests (requires [gtest]):
- `make test`

## Benchmarks

To build and run benchmarks (requires [benchmark]):
- `make benchmark`

Some of them will run automatically and others requires to be explicitly called,
`./bin/benchmark/filename <arguments>`. Examples:
- `./bin/benchmark/zuffix_char_file ./dataset/dna.50MB ./pattern/dna.50MB.*`

## Additional Notes
Binaries which use Google Test and Google Benchmark may use some (optional)
arguments, for more information use `--help`. Binaries which use C++'s STL for
time measurements display the required arguments if no argument is present.

[gtest]: https://github.com/google/googletest "Google Test"
[benchmark]: https://github.com/google/benchmark "Google Benchmark"

[sux]: https://github.com/vigna/sux "Sux"
[sdsl-lite]: https://github.com/simongog/sdsl-lite "Succinct Data Structure Library"
[r-index]: https://github.com/nicolaprezza/r-index "Prezza's r-index"
[CSA++]: https://github.com/mpetri/benchmark-suffix-array "Petri's CSA++"

[experiments]: http://pizzachili.dcc.uchile.cl/experiments.html "Pizza&Chili Experiments"
