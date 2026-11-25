# Building Circle-stdlib on Windows with MSYS2

Building Circle-stdlib on Windows is possible using an MSYS2 **MINGW64**
shell and the Arm GNU Toolchain for AArch64.

This build environment is not officially supported. The instructions
below describe the configuration tested on Windows 11.

## Prerequisites

Install MSYS2 and use its **MINGW64** shell.

The tested build needed:

-   the MSYS version of `make` (see below);
-   `mingw-w64-x86_64-gcc` --- host C/C++ compiler, required by
    `configure` and when building newlib;
-   the Arm GNU Toolchain for AArch64, installed and with its `bin`
    directory in `PATH`. The tested toolchain was
    `aarch64-none-elf-gcc 15.2.Rel1`.

### Important: use the MSYS version of make

Use the MSYS version of `make`, available as `/usr/bin/make` from the
MSYS2 shell; it is the `usr\bin\make.exe` executable inside the MSYS2
installation.

Do not use `mingw32-make` (provided by `mingw-w64-x86_64-make`).

You can check which version is being used with:

``` bash
which make
make --version
```

The expected path is `/usr/bin/make`, and the version output should
report that it was built for `x86_64-pc-cygwin`.

The `configure` script writes MSYS-style absolute paths (for example,
`/d/...`) into `Config.mk`. The MSYS version of `make` understands these
paths. A native MinGW make interprets them incorrectly (for example, as
`D:\d\...`) and the build fails.

## Configure and build

Open an MSYS2 **MINGW64** shell and go to the `circle-stdlib` source
directory. Adapt the configuration options (such as `--aarch64`,
`--raspberrypi`, `--prefix` and `--kernel-max-size`) to your target
platform and build requirements.

The following command is the configuration that was successfully tested:

``` bash
./configure --aarch64 --raspberrypi 3 --prefix aarch64-none-elf- --kernel-max-size 8
make
```

## Tested configuration

The following configuration was successfully built:

-   Windows 11
-   MSYS2, MINGW64 shell
-   Arm GNU Toolchain `aarch64-none-elf-gcc 15.2.Rel1`
-   `libstdc++` / newlib configuration

The complete build, including newlib and Circle, completed successfully.

The LLVM libc++ configuration (`--libcxx`) has not been tested.
