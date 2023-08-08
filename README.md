# Engineering Zuffix Arrays

The following guidelines have been tested in Fedora Linux 38 (Workstation Edition).

## Requirements

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

## Dataset

You can automatically download and extract (requires wget and gunzip)
the Pizza&Chili corpus with:
```
$ make util
$ ./maketexts.sh
```

## Tests and benchmarks
- `make test`: run all the tests.
- `make benchmark`: compile all the benchmarks.

[googletest]: https://github.com/google/googletest "GoogleTest - Google Testing and Mocking Framework"
[benchmark]: https://github.com/google/benchmark "google/benchmark: A microbenchmark support library"
[folly]: https://github.com/facebook/folly "Folly: Facebook Open-source Library"
