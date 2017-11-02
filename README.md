# circle-stdlib

## Overview

The goal of this project is to provide C and C++ standard library support for the
Raspberry Pi bare metal environment [Circle](https://github.com/rsta2/circle).

[Newlib](https://sourceware.org/newlib/) is used as the standard C library. The fork
[circle-newlib](https://github.com/smuehlst/circle-newlib) contains the changes for
building Newlib in combination with Circle.

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

The `build.bash` script has the following options:

```
$ ./build.bash -h
usage: build.bash [ <option> ... ]
Build Circle with newlib standard C library.

Options:
  -c, --clean                    clean build results and exit
  -d, --debug                    build with debug information, without optimizer
  -h, --help                     show usage message
  -n, --no-cpp                   do not support C++ standard library
  --no-parallel-build            don't build in parallel
  -p <string>, --prefix <string> prefix of the toolchain commands (default: arm-none-eabi-)
  -r <number>, --raspberrypi <number>
                                 Circle Raspberry Pi model number (1, 2, 3, default: 1)
  -s <path>, --stddefpath <path>
                                 path where stddef.h header is located (only necessary
                                 if  script cannot determine it automatically)
```

### Building a Sample Program

```
cd samples/01-nosys
make
```

## Current State

v3.0:
* Implemented stdin/stdout/stderr based on Circle's [CConsole](libs/circle/lib/input/console.cpp)
class.
* A build of circle-stdlib with ARM gcc toolchains that include Newlib out-of-the-box now provides
support for C and C++ standard libraries. This is due to the fact that the Newlib provided by the toolchain
is replaced by circle-stdlib's Newlib build, and the toolchain's standard C++ library works with the
replaced Newlib.

Previous releases:

* v2.0: This release implements Newlib's open(), close(), read() and write()
system calls bases on Circle's I/O functions. This enables stdio functionality.
A small [test program](samples/03-stdio-fatfs) demonstrates the use of
stdio file system functions with Circle.

* V1.0: Initial build of Newlib with Circle, without any systems calls being implemented.

## License

This project is licensed under the GNU GENERAL PUBLIC LICENSE
Version 3 - see the [LICENSE](LICENSE) file for details

## Acknowledgements

* Rene Stange for [Circle](https://github.com/rsta2/circle).
* The Newlib team for [Newlib](https://sourceware.org/newlib/).
