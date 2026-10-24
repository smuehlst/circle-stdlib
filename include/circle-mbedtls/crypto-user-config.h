//
// crypto-user-config.h
//
// Circle-specific adjustments to the default TF-PSA-Crypto configuration
// (libs/mbedtls/tf-psa-crypto/include/psa/crypto_config.h).
//
// This file is included after the default configuration via
// TF_PSA_CRYPTO_USER_CONFIG_FILE. Only list deviations from the defaults here.
//
#ifndef _circle_mbedtls_crypto_user_config_h
#define _circle_mbedtls_crypto_user_config_h

// Threading support with Circle's scheduler (see threadingmodule.cpp and
// include/threading_alt.h)
#define MBEDTLS_THREADING_C
#define MBEDTLS_THREADING_ALT

// Entropy is provided by mbedtls_platform_get_entropy() in entropymodule.cpp,
// there is no OS entropy source on bare metal.
#undef MBEDTLS_PSA_BUILTIN_GET_ENTROPY
#define MBEDTLS_PSA_DRIVER_GET_ENTROPY

// There is no reliable real-time clock, so certificate validity dates cannot
// be checked. time() from circle-newlib is still used for TLS timestamps.
#undef MBEDTLS_HAVE_TIME_DATE

// The default mbedtls_ms_time() requires POSIX clock_gettime(), it is
// implemented with Circle's system timer in cryptomodule.cpp.
#define MBEDTLS_PLATFORM_MS_TIME_ALT

// No persistent PSA key store
#undef MBEDTLS_PSA_CRYPTO_STORAGE_C
#undef MBEDTLS_PSA_ITS_FILE_C

// The Armv8-A Cryptographic Extensions can only be detected at runtime on
// Linux. Elsewhere MBEDTLS_AESCE_C uses the instructions unconditionally,
// which faults on the Cortex-A53/A72 cores of the Raspberry Pi 3/4.
#undef MBEDTLS_AESCE_C
#undef MBEDTLS_SHA256_USE_ARMV8_A_CRYPTO_IF_PRESENT
#undef MBEDTLS_SHA512_USE_A64_CRYPTO_IF_PRESENT

// x86-only accelerators
#undef MBEDTLS_AESNI_C

#endif
