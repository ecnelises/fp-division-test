# FP Division Test

Simple tests for floating point division precision of Clang or other compilers.

## How to?

```shell
make CC=<YOUR_COMPILER_PATH> tests
```

Use `make clean` to clean object files.

## What tests?

This consists of three tests:

- `algorithm-test`, for testing accuracy of estimations.
- `compiling-test`, for testing accuracy for real compiled program.
- `compiling-test-set` is the original test in a set of random numbers.
