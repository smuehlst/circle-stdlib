# circle-stdlib

![Status badge for master branch](https://github.com/smuehlst/circle-stdlib/workflows/branch%20%22master%22/badge.svg)
![Status badge for develop branch](https://github.com/smuehlst/circle-stdlib/workflows/branch%20%22develop%22/badge.svg)

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
* gcc ARM toolchain from [Arm Developer GNU-A Downloads](https://developer.arm.com/open-source/gnu-toolchain/gnu-a/downloads) on the path. Successfully tested with:
  * gcc 10.2-2020.11 [32-bit](https://developer.arm.com/-/media/Files/downloads/gnu-a/10.2-2020.11/binrel/gcc-arm-10.2-2020.11-x86_64-arm-none-eabi.tar.xz)/[64-bit](https://developer.arm.com/-/media/Files/downloads/gnu-a/10.2-2020.11/binrel/gcc-arm-10.2-2020.11-x86_64-aarch64-none-elf.tar.xz).

### Building the Libraries

```bash
git clone --recursive https://github.com/smuehlst/circle-stdlib.git
cd circle-stdlib
./configure
make
```

The `configure` script has the following options:

```bash
$ ./configure -h
usage: configure [ <option> ... ]
Configure Circle with newlib standard C library and mbed TLS library.

Options:
  -d, --debug                    build with debug information, without optimizer
  -h, --help                     show usage message
  -n, --no-cpp                   do not support C++ standard library
  --opt-tls                      build with mbed TLS support
  -p <string>, --prefix <string> prefix of the toolchain commands (default: arm-none-eabi-)
  --qemu                         build for running under QEMU in semihosting mode
  -r <number>, --raspberrypi <number>
                                 Circle Raspberry Pi model number (1, 2, 3, 4, default: 1)
  --softfp                       use float ABI setting "softfp" instead of "hard"
  -s <path>, --stddefpath <path>
                                 path where stddef.h header is located (only necessary
                                 if  script cannot determine it automatically)
```

To clean the project directory, the following commands can be used:

```bash
make clean
make mrproper   # removes the configuration too
```

### Building the Samples

```bash
make build-samples
```

## Release History

See [CHANGELOG.md](CHANGELOG.md).

## License

This project is licensed under the GNU GENERAL PUBLIC LICENSE
Version 3 - see the [LICENSE](LICENSE) file for details

## Acknowledgements

* Rene Stange for [Circle](https://github.com/rsta2/circle).
* The Newlib team for [Newlib](https://sourceware.org/newlib/).
* The mbed TLS team for [mbed TLS](https://tls.mbed.org/).
