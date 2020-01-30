# How to run tests and experiments

After cloning the repository it is necessary to download some external
dependencies:
- download and initialize external modules: `git submodule update --init --recursive`
- download and extract Pizza&Chili dataset: `make dataset`
- generate valid random patterns form dataset files:
  1. `make utils`
  2. automatically `make patterns` or manually `./bin/utils/generate_pattern
     <text> <bytes> <symbols> > out`

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
