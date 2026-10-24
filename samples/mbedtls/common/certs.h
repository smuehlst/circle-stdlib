/*
 *  Embedded test certificates for the Mbed TLS samples
 *
 *  Replaces <test/certs.h> from the Mbed TLS test framework, which is not
 *  part of the Mbed TLS libraries. Each sample defines the objects it needs
 *  in its own certs.c.
 */
#ifndef _samples_mbedtls_certs_h
#define _samples_mbedtls_certs_h

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Concatenation of the RSA and EC test CA certificates in PEM format */
extern const char mbedtls_test_cas_pem[];
extern const size_t mbedtls_test_cas_pem_len;

/* RSA test server certificate (CN=localhost) and key in PEM format */
extern const char mbedtls_test_srv_crt[];
extern const size_t mbedtls_test_srv_crt_len;
extern const char mbedtls_test_srv_key[];
extern const size_t mbedtls_test_srv_key_len;

#ifdef __cplusplus
}
#endif

#endif
