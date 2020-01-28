# How to run tests and experiments

After cloning the repository you should download some external dependencies:
- download and initialize external modules: `git submodule update --init --recursive`
- download and extract Pizza&Chili dataset: `make dataset`

## Test

To build and run tests (requires [gtest]):
- `make test`

## Benchmark

To build and run benchmarks (requires [gbench]):
- `make benchmark`
- `./bin/benchmark/filename <arguments>`

Examples:
- `./bin/benchmark/zuffix_find_dna ./dataset/dna.50MB 10 100 1000 10000`

# Additional Notes
Binaries produced using Google Test and Google Benchmark may use some
(optionals) arguments, for more information use `--help`. Binaries which use
C++'s STL for time measurements display the required arguments if no argument is
present.


[gtest]: https://github.com/google/googletest "Google Test"
[gbench]: https://github.com/google/benchmark "Google Benchmark"
