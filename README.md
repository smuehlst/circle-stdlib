# circle-stdlib

![Status badge for master branch](https://github.com/smuehlst/circle-stdlib/actions/workflows/master.yaml/badge.svg?branch=master)
![Status badge for develop branch](https://github.com/smuehlst/circle-stdlib/actions/workflows/develop.yaml/badge.svg?branch=develop)

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

A toolchain from [Arm GNU Toolchain Downloads](https://developer.arm.com/downloads/-/arm-gnu-toolchain-downloads):

* gcc 14.3.Rel1 hosted on Intel Linux or Windows Subsystem for Linux (WSL/WSL2) (build is tested with [GitHub Action](https://github.com/smuehlst/circle-stdlib/actions)):
  * [AArch32 bare-metal target (arm-none-eabi)](https://developer.arm.com/-/media/Files/downloads/gnu/14.3.rel1/binrel/arm-gnu-toolchain-14.3.rel1-x86_64-arm-none-eabi.tar.xz)
  * [AArch64 ELF bare-metal target (aarch64-none-elf)](https://developer.arm.com/-/media/Files/downloads/gnu/14.3.rel1/binrel/arm-gnu-toolchain-14.3.rel1-x86_64-aarch64-none-elf.tar.xz)
* gcc 14.3.Rel1 hosted on 64-bit Linux on Raspberry Pi (not tested, but reported to work):
  * [AArch32 bare-metal target (arm-none-eabi)](https://developer.arm.com/-/media/Files/downloads/gnu/14.3.rel1/binrel/arm-gnu-toolchain-14.3.rel1-aarch64-arm-none-eabi.tar.xz)
  * [AArch64 ELF bare-metal target (aarch64-none-elf)](https://developer.arm.com/-/media/Files/downloads/gnu/14.3.rel1/binrel/arm-gnu-toolchain-14.3.rel1-aarch64-aarch64-none-elf.tar.xz)

### Building the Libraries

Add the toolchain to the path, then:

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
  -o, --option <name>[=<value>]  additional preprocessor define (optionally with value)
                                 can be repeated
  --opt-tls                      build with mbed TLS support
  -p <string>, --prefix <string> prefix of the toolchain commands (default: arm-none-eabi-)
  --qemu                         build for running under QEMU in semihosting mode
  -r <number>, --raspberrypi <number>
                                 Circle Raspberry Pi model number (1, 2, 3, 4, 5, default: 1)
  --softfp                       use float ABI setting "softfp" instead of "hard"
  -s <path>, --stddefpath <path>
                                 path where stddef.h header is located (only necessary
                                 if script cannot determine it automatically)
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

### Running and Debugging the Programs

The resulting executables are normal Circle bare-metal applications. Circle's
standard [installation](https://github.com/rsta2/circle#installation) 
and [debugging](https://github.com/rsta2/circle/blob/master/doc/debug.txt)
instructions apply.

For running the programs under QEMU see Circle's corresponding
[notes on QEMU](https://github.com/rsta2/circle/blob/master/doc/qemu.txt).

## Release History

See [CHANGELOG.md](CHANGELOG.md).

## License

This project is licensed under the GNU GENERAL PUBLIC LICENSE
Version 3 - see the [LICENSE](LICENSE) file for details

## Acknowledgements

* Rene Stange for [Circle](https://github.com/rsta2/circle).
* The Newlib team for [Newlib](https://sourceware.org/newlib/).
* The mbed TLS team for [mbed TLS](https://tls.mbed.org/).
