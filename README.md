# Engineering Zuffix Arrays

We recommend using an updated GNU Linux distribution (e.g., Fedora Linux 38 Workstation Edition).

## Installing the requirements

Install [googletest], [benchmark] (the -devel packages), and all the
dependencies of [folly]. The dependencies of [folly] can be listed
with:
```
$ git clone https://github.com/facebook/folly.git
$ cd folly
$ git checkout 4dcb3c6d1
$ ./build/fbcode_builder/getdeps.py install-system-deps --dry-run --recursive
```
or by following the guidelines at [https://github.com/facebook/folly#dependencies](https://github.com/facebook/folly#dependencies).

Fetch and compile (with optimizations enabled) all the other
dependencies using `./makedeps.sh`.

## Downloading the dataset

You can automatically download and extract (requires wget and gunzip)
the Pizza&Chili corpus with:
```
$ make util
$ ./maketexts.sh
```

## Building the project

We are using g++ (GCC) 13.2.1. You can run a set of tests and compile all the benchmarks with:
```
$ make test
$ make benchmark
```

## Benchmarks and examples

The following is a brief (and not exhaustive) list that explain how to run some of the benchmarks:

- interactive (exit with `crtl+c`) search queries:
  ```
  $ ./bin/benchmark/interactive/enhanced ./texts/english-50MB.txt                        # enhanced
  $ ./bin/benchmark/interactive/memcmp-zuffix-crc32cfolly ./texts/english-50MB.txt       # memcmp-folly
  $ ./bin/benchmark/interactive/signature-zuffix-crc32cfolly ./texts/english-50MB.txt    # signature-folly
  ```
- search random patterns with at least one occurrence in the text:
  ```
  $ ./bin/benchmark/findrandom/enhanced ./texts/dna-200MB.txt                            # enhanced
  $ ./bin/benchmark/findrandom/memcmp-zuffix-xxh3 ./texts/dna-200MB.txt                  # memcmp-xxh3
  $ ./bin/benchmark/findrandom/memcmp-zuffix-wyhash ./texts/dna-200MB.txt                # memcmp-wyhash
  ```
  
- search random patterns with no occurrence in the text:
  ```
  $ ./bin/benchmark/nofindrandom/enhanced ./texts/proteins-200MB.txt                     # enhanced
  $ ./bin/benchmark/nofindrandom/signature-zuffix-crc32zlib ./texts/proteins-200MB.txt   # signature-zlib
  $ ./bin/benchmark/nofindrandom/signature-zuffix-crc32cfolly ./texts/proteins-200MB.txt # signature-folly
  ```

[googletest]: https://github.com/google/googletest "GoogleTest - Google Testing and Mocking Framework"
[benchmark]: https://github.com/google/benchmark "google/benchmark: A microbenchmark support library"
[folly]: https://github.com/facebook/folly "Folly: Facebook Open-source Library"
