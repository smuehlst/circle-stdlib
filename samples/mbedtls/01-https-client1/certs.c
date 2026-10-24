/*
 *  Embedded test certificates (see ../common/certs.h)
 */
#include "../common/certs.h"
#include "../common/test_certs.h"

const char mbedtls_test_cas_pem[] = TEST_CA_CRT_RSA_SHA256_PEM TEST_CA_CRT_EC_PEM;
const size_t mbedtls_test_cas_pem_len = sizeof mbedtls_test_cas_pem;
