# circle-stdlib

## Overview

The goal of this project is to provide C and C++ standard library support for the
Raspberry Pi bare metal environment [Circle](https://github.com/rsta2/circle).

[Newlib](https://sourceware.org/newlib/) is used as the standard C library. The fork
[circle-newlib](https://github.com/smuehlst/circle-newlib) contains the changes for
building Newlib in combination with Circle.

[mbed TLS](https://tls.mbed.org/) can optionally be used for TLS connections in
Circle (call configure with `--opt-tls`, see also the
[README file for circle-mbedtls](circle-mbedtls.md)).

## Getting Started

### Prerequisites

* Linux or Windows 10 Subsystem for Linux (WSL).
* gcc ARM toolchain on the path. Successfully tested with:
  * gcc version 7.3.1 20180622: gcc-arm-none-eabi-7-2018-q2-update-linux.tar.bz2 from https://developer.arm.com/open-source/gnu-toolchain/gnu-rm/downloads
  * gcc version 8.2-2019.01 32-bit and 64-bit:
    * gcc-arm-8.2-2019.01-x86_64-arm-eabi.tar.xz from https://developer.arm.com/open-source/gnu-toolchain/gnu-a/downloads
    * gcc-arm-8.2-2019.01-x86_64-aarch64-elf.tar.xz from https://developer.arm.com/open-source/gnu-toolchain/gnu-a/downloads

### Building the Libraries

```
git clone --recursive https://github.com/smuehlst/circle-stdlib.git
cd circle-stdlib
./configure
make
```

The `configure` script has the following options:

```
$ ./configure -h
usage: configure [ <option> ... ]
Configure Circle with newlib standard C library and mbed TLS library.

Options:
  -d, --debug                    build with debug information, without optimizer
  -h, --help                     show usage message
  -n, --no-cpp                   do not support C++ standard library
  -p <string>, --prefix <string> prefix of the toolchain commands (default: arm-none-eabi-)
  -r <number>, --raspberrypi <number>
                                 Circle Raspberry Pi model number (1, 2, 3, default: 1)
  --softfp                       use float ABI setting "softfp" instead of "hard"
  -s <path>, --stddefpath <path>
                                 path where stddef.h header is located (only necessary
                                 if  script cannot determine it automatically)
  --opt-tls                      build with mbed TLS support
```

To clean the project directory, the following commands can be used:

```
make clean
make mrproper		# removes the configuration too
```

### Building the Samples

```
make build-samples
```

## Current Release [v9.0](https://github.com/smuehlst/circle-stdlib/releases/tag/v9.0)

* Synced with [Circle Step 39.1](https://github.com/rsta2/circle/releases/tag/Step39.1).
* AArch64 support.
* Removed deprecated build script `build.bash` (use `configure` and `make` instead).

## Previous Releases

### [v8.0](https://github.com/smuehlst/circle-stdlib/releases/tag/v8.0)

* Synced with [Circle Step 39](https://github.com/rsta2/circle/releases/tag/Step39).
* [mbed TLS](libs/mbedtls) support updated to release 2.16.0.

### [v7.0](https://github.com/smuehlst/circle-stdlib/releases/tag/v7.0)

* Synced with [Circle Step 37](https://github.com/rsta2/circle/releases/tag/Step37).
* [mbed TLS](libs/mbedtls) support updated to release 2.14.1.
* Circle is now included from its default repository. To sync an existing clone of the circle-stdlib repository, enter the following commands:

```
git submodule sync libs/circle
git submodule update --init --remote libs/circle
```

### [v6.0](https://github.com/smuehlst/circle-stdlib/releases/tag/v6.0)

* Synced with [Circle Step 36](https://github.com/rsta2/circle/releases/tag/Step36).

### [v5.0](https://github.com/smuehlst/circle-stdlib/releases/tag/v5.0)

* Synced with [Circle Step 35.1](https://github.com/rsta2/circle/releases/tag/Step35.1).
* Added support for <dirent.h> functions.
* Added support for time()-related functions.
* [mbed TLS](libs/mbedtls) support implemented by Rene Stange.
* Added `configure` script and `Makefile` to replace `build.bash`. build.bash is
  deprecated and will be removed in a future release.

### [v4.0](https://github.com/smuehlst/circle-stdlib/tree/v4.0)

* Synced with [Circle Step 34](https://github.com/rsta2/circle/releases/tag/Step34).
* Added CStdlibApp, CStdlibAppScreen, CStdlibAppStdio and CStdlibAppNetwork for
easier application creation (thanks to Rene Stange).

### [v3.0](https://github.com/smuehlst/circle-stdlib/tree/V3.0)

* Synced with [Circle Step 33](https://github.com/rsta2/circle/releases/tag/Step33).
* Implemented stdin/stdout/stderr based on Circle's CConsole class.
* A build of circle-stdlib with ARM gcc toolchains that include Newlib out-of-the-box now provides
support for C and C++ standard libraries. This is due to the fact that the Newlib provided by the toolchain
is replaced by circle-stdlib's Newlib build, and the toolchain's standard C++ library works with the
replaced Newlib.

### [v2.0](https://github.com/smuehlst/circle-stdlib/tree/v2.0)

This release implements Newlib's open(), close(), read() and write()
system calls bases on Circle's I/O functions. This enables stdio functionality.
A small [test program](samples/03-stdio-fatfs) demonstrates the use of
stdio file system functions with Circle.

### [v1.0](https://github.com/smuehlst/circle-stdlib/tree/v1.0)

Initial build of Newlib with Circle, without any systems calls being implemented.

## License

This project is licensed under the GNU GENERAL PUBLIC LICENSE
Version 3 - see the [LICENSE](LICENSE) file for details

## Acknowledgements

* Rene Stange for [Circle](https://github.com/rsta2/circle).
* The Newlib team for [Newlib](https://sourceware.org/newlib/).
* The mbed TLS team for [mbed TLS](https://tls.mbed.org/).
