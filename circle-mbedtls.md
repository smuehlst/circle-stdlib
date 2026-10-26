# circle-mbedtls

## Overview

circle-mbedtls uses the [Mbed TLS](https://www.trustedfirmware.org/projects/mbed-tls/)
library to provide SSL/TLS support for the Raspberry Pi bare metal environment
[Circle](https://github.com/rsta2/circle).

circle-mbedtls is part of the circle-stdlib project, because it needs the
standard C library support, provided by this project and to reduce the number of
required sub-modules needed to build a circle-mbedtls application.

## Getting Started

### Prerequisites

Same as for the circle-stdlib project. In addition:

* The *libs/mbedtls* submodule contains the nested submodules *framework* and
  *tf-psa-crypto*. Clone with `--recursive` or run
  `git submodule update --init --recursive`.
* Mbed TLS is built with CMake (3.20.2 or later) and Ninja. The build generates
  source files, which requires `python3` with the modules listed in
  *libs/mbedtls/scripts/driver.requirements.txt* (`jinja2`, `jsonschema`) and
  `perl`.

### Building the Libraries

circle-mbedtls uses the circle-stdlib build system. You have to specify the
`--opt-tls` option to enable the SSL/TLS support. The Mbed TLS libraries are
built in *build/mbedtls*.

The Circle-specific configuration is applied on top of the Mbed TLS default
configuration by the files *include/circle-mbedtls/mbedtls-user-config.h*
(TLS and X.509) and *include/circle-mbedtls/crypto-user-config.h*
(TF-PSA-Crypto). Applications must be compiled with `$(MBEDTLS_INCLUDES)` and
`$(MBEDTLS_DEFINES)` from *Config.mk* and linked with `$(MBEDTLS_LIBS)` (see
*samples/mbedtls/Rules.mk*).

### Building a Sample Program

The circle-mbedtls sample programs can be found in the *samples/mbedtls*
subdirectory. Go to the subdirectory of the sample of your choice and do `make`.

## Current State

V2.0:

* Uses Mbed TLS 4.2.0 (with TF-PSA-Crypto 1.2.0). Please read the
  [Mbed TLS 4.0 migration guide](libs/mbedtls/docs/4.0-migration-guide.md) and the
  [TF-PSA-Crypto 1.0 migration guide](libs/mbedtls/tf-psa-crypto/docs/1.0-migration-guide.md)
  before porting applications, which use the Mbed TLS C API directly.
* Mbed TLS is built with its CMake build system.
* Only TLS 1.2 and TLS 1.3 are supported. The removed protocol versions and
  cipher suites (TLS 1.0/1.1, RC4, 3DES, static RSA/ECDH key exchange) are no
  longer available.
* The PSA crypto subsystem must be initialized with `psa_crypto_init()` before
  using any TLS or X.509 function. `CTLSSimpleSupport` does this via the new
  class `CCryptoModule`. Applications, which use the C API without
  `CTLSSimpleSupport`, must instantiate `CThreadingModule` and `CEntropyModule`
  (in this order) and then call `psa_crypto_init()` themselves.
* The random number generator is internal to PSA now. `CEntropyContext`,
  `CCTR_DRBG` and `CTLSConfig::SetRNG()` have been removed, as well as the
  `pPersonalizationString` parameter of `CTLSSimpleClientSocket::Setup()`.
* The PK and elliptic curve identifiers are private in Mbed TLS 4.x. The
  `CX509CertificateProfile` methods, which take such an identifier, have been
  removed. The bit mask methods are still supported.
* The validity dates of certificates are not checked (`MBEDTLS_HAVE_TIME_DATE`
  is disabled), as before.
* The samples 01-https-client1 and 03-https-server1 have been re-ported from the
  Mbed TLS 4.2.0 sources. The samples 02-https-client2 and 04-https-server2 have
  been removed.

## Previous Releases

V1.15:

* Uses Mbed TLS 2.28.10
* Enable threading support
* CThreadingModule must be instantiated early in the app, if CTLSSimpleSupport is not.

V1.14:

* Fix issue in mqttclient.cpp, which caused a system crash, when a disconnect
  message was received from peer.

V1.13:

* Uses Mbed TLS 2.28.9
* Fix issue in mqttclient.cpp, which caused an invalid payload delivered,
  when multiple publish messages arrived at once.

V1.12:

* Uses Mbed TLS 2.28.8

V1.11:

* Uses Mbed TLS 2.18.4
* Update certificate for sample 06-webclient

V1.10:

* Uses Mbed TLS 2.18.3

V1.9:

* Uses Mbed TLS 2.16.12

V1.8:

* Uses Mbed TLS 2.16.11
* Sample 06-webclient sync'ed with changes on www.raspberry.com.

V1.7:

* Uses Mbed TLS 2.16.10
* Sample 06-webclient sync'ed with changes on www.raspberry.org.

V1.6:

* Uses Mbed TLS 2.16.8
* Sample 06-webclient updated to access revision tables from www.raspberry.org.

V1.5:

* Sample programs prepared for (optional) WLAN operation.

V1.4:

* Uses Mbed TLS 2.16.4
* Quality of random numbers for QEMU test improved

V1.3:

* Uses mbed TLS 2.16.2

V1.2:

* Uses mbed TLS 2.16.0

V1.1:

* Uses mbed TLS 2.14.1

V1.0:

* Initial version
* Uses mbed TLS 2.8.0
* Supports a subset of the mbed TLS C API
* Initial attempt to support a C++ API, which is easier to be used

## License

This project is licensed under the GNU GENERAL PUBLIC LICENSE
Version 3 - see the [LICENSE](LICENSE) file for details

## Acknowledgements

* Stephan Muehlstrasser for circle-stdlib, for advise
  and for opening his repository for the circle-mbedtls development.
* The Mbed TLS team for [Mbed TLS](https://www.trustedfirmware.org/projects/mbed-tls/).
