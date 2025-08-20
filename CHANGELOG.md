# Release History

## [v17](https://github.com/smuehlst/circle-stdlib/releases/tag/v17)

* Synced with [Circle Step 50](https://github.com/rsta2/circle/releases/tag/Step50).
* Updated [circle-stdlib's copy](libs/circle-newlib/) of Newlib to version 4.5.0.
* Aligned supported toolchain version with Circle (ARM GNU toolchain 14.3.Rel1).

## [v16.8](https://github.com/smuehlst/circle-stdlib/releases/tag/v16.8)

* Fixed issue #45: The Circle library was not built in parallel mode if the top-level make 
was invoked with `-j`.
* Synced with [Circle Step 49.0.1](https://github.com/rsta2/circle/releases/tag/Step49.0.1).
* Update Mbed TLS to [release v2.28.10](https://github.com/Mbed-TLS/mbedtls/releases/tag/mbedtls-2.28.10).

## [v16.7](https://github.com/smuehlst/circle-stdlib/releases/tag/v16.7)

* Synced with [Circle Step 49](https://github.com/rsta2/circle/releases/tag/Step49).
* Fixed a crash in the MQTT client.

## [v16.6](https://github.com/smuehlst/circle-stdlib/releases/tag/v16.6)

* Synced with [Circle Step 48](https://github.com/rsta2/circle/releases/tag/Step48).
* Update Mbed TLS to [release v2.28.9](https://github.com/Mbed-TLS/mbedtls/releases/tag/mbedtls-2.28.9).
* Fixed invalid payload in MQTT client.
* Fixed broken GitHub action caused by missing dependencies of the old QEMU 6.0.0 release. 

## [v16.5](https://github.com/smuehlst/circle-stdlib/releases/tag/v16.5)

* Synced with [Circle Step 47](https://github.com/rsta2/circle/releases/tag/Step47).
* Update Mbed TLS to [release v2.28.8](https://github.com/Mbed-TLS/mbedtls/releases/tag/v2.28.8).
* Aligned toolchain version with Circle (ARM GNU toolchain 13.2.Rel1).

## [v16.4](https://github.com/smuehlst/circle-stdlib/releases/tag/v16.4)

* Fixed issue #35 "No output from printf() in samples/02-stdio-hello with v16.3".

## [v16.3](https://github.com/smuehlst/circle-stdlib/releases/tag/v16.3)

* Synced with [Circle Step 46](https://github.com/rsta2/circle/releases/tag/Step46).
* Fixed duplicate definition of fcntl().

## [v16.2](https://github.com/smuehlst/circle-stdlib/releases/tag/v16.2)

* Synced with [Circle Step 45.3.1](https://github.com/rsta2/circle/releases/tag/Step45.3.1).
* Update Mbed TLS to [release v2.28.4](https://github.com/Mbed-TLS/mbedtls/releases/tag/v2.28.4).

## [v16.1](https://github.com/smuehlst/circle-stdlib/releases/tag/v16.1)

* Synced with [Circle Step 45.2](https://github.com/rsta2/circle/releases/tag/Step45.2).
* Update mbed TLS to [release v2.28.3](https://github.com/Mbed-TLS/mbedtls/releases/tag/v2.28.3).
* Aligned toolchain version with Circle (ARM GNU toolchain 12.2.Rel1).

## [v16](https://github.com/smuehlst/circle-stdlib/releases/tag/v16)

* Synced with [Circle Step 45.1](https://github.com/rsta2/circle/releases/tag/Step45.1).
* Implemented functions that previously returned an error code:
  * `getcwd()` and `chdir()`
  * `dup()`, `dup2()` and `fcntl(... F_DUPFD ...)` (Note: Only the F_DUPFD command is implemented for fcntl()!)
  * `stat()` and `fstat()`
  * `fsync()`
  * `ftruncate()`
  * `isatty()`

## [v15.14](https://github.com/smuehlst/circle-stdlib/releases/tag/v15.14)

* Synced with [Circle Step 45](https://github.com/rsta2/circle/releases/tag/Step45).
* Updated mbed TLS to [release v2.28.1](https://github.com/Mbed-TLS/mbedtls/releases/tag/v2.28.1).

## [v15.13](https://github.com/smuehlst/circle-stdlib/releases/tag/v15.13)

* Synced with [Circle Step 44.5](https://github.com/rsta2/circle/releases/tag/Step44.5).

## [v15.12](https://github.com/smuehlst/circle-stdlib/releases/tag/v15.12)

* Synced with [Circle Step 44.4.1](https://github.com/rsta2/circle/releases/tag/Step44.4.1).
* Updated mbed TLS to [release v2.16.12](https://github.com/ARMmbed/mbedtls/releases/tag/v2.16.12).
* New `configure` command line option `--option` for specifying additional preprocessor defines
  consistently across all libraries.

## v15.11

Faulty release that was intended to include Circle step 44.4.1, but the
commit was forgotten. Therefore release v15.11 is superseded by v15.12.

## [v15.10](https://github.com/smuehlst/circle-stdlib/releases/tag/v15.10)

* Synced with [Circle Step 44.3](https://github.com/rsta2/circle/releases/tag/Step44.3).

## [v15.9](https://github.com/smuehlst/circle-stdlib/releases/tag/v15.9)

* Synced with [Circle Step 44.2](https://github.com/rsta2/circle/releases/tag/Step44.2).
* Aligned toolchain version with Circle (gcc 10.3-2021.07).
* Updated mbed TLS to [release 2.16.11](https://github.com/ARMmbed/mbedtls/releases/tag/v2.16.11).

## [v15.8](https://github.com/smuehlst/circle-stdlib/releases/tag/v15.8)

* Synced with [Circle Step 44.1](https://github.com/rsta2/circle/releases/tag/Step44.1).
* Fixed issue #22 "Bad memmove behavior". This issue was only reproducible with the AArch64 build.

## [v15.7](https://github.com/smuehlst/circle-stdlib/releases/tag/v15.7)

* Synced with [Circle Step 44](https://github.com/rsta2/circle/releases/tag/Step44).

## [v15.6](https://github.com/smuehlst/circle-stdlib/releases/tag/v15.6)

* Synced with [Circle Step 43.3](https://github.com/rsta2/circle/releases/tag/Step43.3).
* Aligned toolchain version with Circle (gcc 10.2-2020.11).

## [v15.5](https://github.com/smuehlst/circle-stdlib/releases/tag/v15.5)

* Synced with [Circle Step 43.2](https://github.com/rsta2/circle/releases/tag/Step43.2).
* Added boolean parameter bWaitForActivate for CStdlibAppNetwork::Initialize() (issue #15).

## [v15.4](https://github.com/smuehlst/circle-stdlib/releases/tag/v15.4)

* Fixed issue #14: fopen() returned non-NULL value when opening non-existent
file for reading.

## [v15.3](https://github.com/smuehlst/circle-stdlib/releases/tag/v15.3)

* Synced with [Circle Step 43.1](https://github.com/rsta2/circle/releases/tag/Step43.1).
* Updated mbedtls to [release 2.16.8](https://github.com/ARMmbed/mbedtls/releases/tag/mbedtls-2.16.8).

## [v15.2](https://github.com/smuehlst/circle-stdlib/releases/tag/v15.2)

* Synced with [Circle Step 43](https://github.com/rsta2/circle/releases/tag/Step43).

## [v15.1](https://github.com/smuehlst/circle-stdlib/releases/tag/v15.1)

* Synced with [Circle Step 42.1](https://github.com/rsta2/circle/releases/tag/Step42.1).
* Support for new WLAN features of Circle.

## [v15](https://github.com/smuehlst/circle-stdlib/releases/tag/v15)

* Additional file-related capabilities of [FatFs](http://elm-chan.org/fsw/ff/00index_e.html) available now:
  * File seeking in stdio and in the C++ standard library.
  * Directory operations rename(), mkdir() and unlink().
* Extended the smoke test run by the [GitHub action](https://github.com/smuehlst/circle-stdlib/actions) with basic tests for file operations.

## [v14](https://github.com/smuehlst/circle-stdlib/releases/tag/v14)

* Synced with [Circle Step 42](https://github.com/rsta2/circle/releases/tag/Step42).
* Switched the FAT filesystem implementation to [FatFs](http://elm-chan.org/fsw/ff/00index_e.html).
  While FatFs offers more features than Circle's FAT filesystem implementation that was used previously,
  this release of circle-stdlib only aims to be compatible with previous releases regarding
  filesystem-related functionality.

## [v13.2](https://github.com/smuehlst/circle-stdlib/releases/tag/v13.2)

* Synced with [Circle Step 41.2](https://github.com/rsta2/circle/releases/tag/Step41.2).

## [v13.1](https://github.com/smuehlst/circle-stdlib/releases/tag/v13.1)

* Synced with [Circle Step 41.1](https://github.com/rsta2/circle/releases/tag/Step41.1).
* Minor fix in the README.md file.

## [v13.0](https://github.com/smuehlst/circle-stdlib/releases/tag/v13.0)

* Synced with [Circle Step 41](https://github.com/rsta2/circle/releases/tag/Step41).
* Updated mbedtls to [release 2.16.4](https://github.com/ARMmbed/mbedtls/releases/tag/mbedtls-2.16.4).
* New [configure](configure) option `--qemu` to build with support for QEMU semihosting mode.
* Added [GitHub action](https://github.com/smuehlst/circle-stdlib/actions) that builds
  with the supported compilers in 32-bit and 64-bit mode and runs a smoke test under QEMU.
* Minor changes to circle-stdlib and mbedtls samples so they terminate under QEMU.

Thanks to Rene Stange for making it possible to run Circle programs under QEMU in
a fully automated manner.

## [v12.0](https://github.com/smuehlst/circle-stdlib/releases/tag/v12.0)

* Updated circle-newlib from upstream to fix a compilation error in the C++ sample when using gcc 8.3-2019.03.

## [v11.0](https://github.com/smuehlst/circle-stdlib/releases/tag/v11.0)

* Synced with [Circle Step 40](https://github.com/rsta2/circle/releases/tag/Step40).
* Raspberry Pi 4 support.

## [v10.0](https://github.com/smuehlst/circle-stdlib/releases/tag/v10.0)

* Synced with [Circle Step 39.2](https://github.com/rsta2/circle/releases/tag/Step39.2).

## [v9.0](https://github.com/smuehlst/circle-stdlib/releases/tag/v9.0)

* Synced with [Circle Step 39.1](https://github.com/rsta2/circle/releases/tag/Step39.1).
* AArch64 support.
* Removed deprecated build script `build.bash` (use `configure` and `make` instead).

## [v8.0](https://github.com/smuehlst/circle-stdlib/releases/tag/v8.0)

* Synced with [Circle Step 39](https://github.com/rsta2/circle/releases/tag/Step39).
* [mbed TLS](libs/mbedtls) support updated to release 2.16.0.

## [v7.0](https://github.com/smuehlst/circle-stdlib/releases/tag/v7.0)

* Synced with [Circle Step 37](https://github.com/rsta2/circle/releases/tag/Step37).
* [mbed TLS](libs/mbedtls) support updated to release 2.14.1.
* Circle is now included from its default repository. To sync an existing clone of the circle-stdlib repository, enter the following commands:

```bash
git submodule sync libs/circle
git submodule update --init --remote libs/circle
```

## [v6.0](https://github.com/smuehlst/circle-stdlib/releases/tag/v6.0)

* Synced with [Circle Step 36](https://github.com/rsta2/circle/releases/tag/Step36).

## [v5.0](https://github.com/smuehlst/circle-stdlib/releases/tag/v5.0)

* Synced with [Circle Step 35.1](https://github.com/rsta2/circle/releases/tag/Step35.1).
* Added support for <dirent.h> functions.
* Added support for time()-related functions.
* [mbed TLS](libs/mbedtls) support implemented by Rene Stange.
* Added `configure` script and `Makefile` to replace `build.bash`. build.bash is
  deprecated and will be removed in a future release.

## [v4.0](https://github.com/smuehlst/circle-stdlib/tree/v4.0)

* Synced with [Circle Step 34](https://github.com/rsta2/circle/releases/tag/Step34).
* Added CStdlibApp, CStdlibAppScreen, CStdlibAppStdio and CStdlibAppNetwork for
easier application creation (thanks to Rene Stange).

## [v3.0](https://github.com/smuehlst/circle-stdlib/tree/V3.0)

* Synced with [Circle Step 33](https://github.com/rsta2/circle/releases/tag/Step33).
* Implemented stdin/stdout/stderr based on Circle's CConsole class.
* A build of circle-stdlib with ARM gcc toolchains that include Newlib out-of-the-box now provides
support for C and C++ standard libraries. This is due to the fact that the Newlib provided by the toolchain
is replaced by circle-stdlib's Newlib build, and the toolchain's standard C++ library works with the
replaced Newlib.

## [v2.0](https://github.com/smuehlst/circle-stdlib/tree/v2.0)

This release implements Newlib's open(), close(), read() and write()
system calls bases on Circle's I/O functions. This enables stdio functionality.
A small [test program](samples/03-stdio-fatfs) demonstrates the use of
stdio file system functions with Circle.

## [v1.0](https://github.com/smuehlst/circle-stdlib/tree/v1.0)

Initial build of Newlib with Circle, without any systems calls being implemented.
