//
// mbedtls-user-config.h
//
// Circle-specific adjustments to the default Mbed TLS (TLS and X.509)
// configuration (libs/mbedtls/include/mbedtls/mbedtls_config.h).
//
// This file is included after the default configuration via
// MBEDTLS_USER_CONFIG_FILE. Only list deviations from the defaults here.
// Crypto and platform options belong in crypto-user-config.h.
//
#ifndef _circle_mbedtls_mbedtls_user_config_h
#define _circle_mbedtls_mbedtls_user_config_h

// The mbedtls_net_*() functions are implemented on top of Circle's TCP/IP
// stack in src/circle-mbedtls/net_sockets.cpp.
#undef MBEDTLS_NET_C

// library/timing.c requires POSIX timers or Windows
#undef MBEDTLS_TIMING_C

#endif
