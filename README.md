[![Build and test](https://github.com/ooonak/sc/actions/workflows/ci.yaml/badge.svg)](https://github.com/ooonak/sc/actions/workflows/ci.yaml)
[![Static analysis](https://github.com/ooonak/sc/actions/workflows/static-analysis.yaml/badge.svg)](https://github.com/ooonak/sc/actions/workflows/static-analysis.yaml)
[![Dynamic analysis](https://github.com/ooonak/sc/actions/workflows/dynamic-analysis.yaml/badge.svg)](https://github.com/ooonak/sc/actions/workflows/dynamic-analysis.yaml)

# Simple Communicator (SC)
C++ library for connection oriented message based communication over unix domain sockets.

## Development

### Setup

```
$ conan new my_cmake_lib -d name=sc -d version=0.0.1 -d requires=spdlog/1.15.0
```

### Build

```
$ conan install . --profile=x86_64-clang-18-debug --build=missing
$ cmake --preset conan-debug
$ cd build/Debug
$ cmake --build .
$ ctest
```

### Analysis

```
$ find src include -type f \( -name '*.cpp' -o -name '*.hpp' \) | xargs clang-tidy -p build/Debug -header-filter='^$(pwd)/src/|^$(pwd)/include/'
$ cppcheck --suppressions-list=CppCheckSuppressions.txt --project=build/Debug/compile_commands.json --error-exitcode=1 --suppress=missingIncludeSyst
em --quiet
$ valgrind --leak-check=full --track-origins=yes --trace-children=yes build/Debug/tests/scTests
```
