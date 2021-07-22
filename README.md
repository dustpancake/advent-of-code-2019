# Advent of Code 2019

My solutions to the [Advent of Code](https://adventofcode.com/) 2019.

- requires Conan package manager

Dependencies are installed with 
```bash
mkdir -p build && cd build && conan install ..
```

## Building

Each day contains its own Makefile, and can be build from within its own directory. 

To build all days at once, from the root directory
```bash
make
```
Or specific days, e.g. day 13, with 
```bash
make day13
``` 
The `opcomp` unit tests can be run with
```
make test
```