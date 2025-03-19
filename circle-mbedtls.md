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

Same as for the circle-stdlib project.

### Building the Libraries

circle-mbedtls uses the circle-stdlib build system. You have to specify the
`--opt-tls` option to enable the SSL/TLS support.

### Building a Sample Program

The circle-mbedtls sample programs can be found in the *samples/mbedtls*
subdirectory. Go to the subdirectory of the sample of your choice and do `make`.

## Current State

V1.14:

* Fix issue in mqttclient.cpp, which caused a system crash, when a disconnect
  message was received from peer.

## Previous Releases

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
