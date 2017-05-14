# circle-stdlib

## Overview

The goal of this project is to build C and C++ standard libraries for use with the
Raspberry Pi bare metal environment [Circle](https://github.com/rsta2/circle).

[Newlib](https://sourceware.org/newlib/) is used as the standard C library.

## Getting Started

### Prerequisites

* Linux or Windows 10 Subsystem for Linux (WSL).
* gcc ARM toolchain on the path.

### Building the Libraries

Clone the `circle-stdlib` repository with its submodules and run
the script `build.bash` to build the libraries:

```
git clone --recursive https://github.com/smuehlst/circle-stdlib.git
cd circle-stdlib
./build.bash
```

### Building a Sample Program

```
cd samples/01-nosys
make
```

## Current State

As a first step Newlib is built without any system calls being implemented. This means that
all features that do not depend on system calls should work (e.g. math functions,
`setjmp`/`longjmp`, `bsearch`, `qsort`), but everything that depends on "system calls" like
`open`, `read` etc. does not work and return errors (e.g. stdio functions).

## License

This project is licensed under the GNU GENERAL PUBLIC LICENSE
Version 3 - see the [LICENSE.md](LICENSE.md) file for details

## Acknowledgements

* Rene Stange for [Circle](https://github.com/rsta2/circle).
* The Newlib team for [Newlib](https://sourceware.org/newlib/).
