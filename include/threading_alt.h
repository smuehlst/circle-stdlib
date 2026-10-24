#ifndef _threading_alt_h
#define _threading_alt_h

// Platform types for MBEDTLS_THREADING_ALT (included by <mbedtls/threading.h>).
// See: circle-mbedtls/threadingmodule.h

typedef struct
{
	void *pMutex;		// opaque pointer to a CMutex object
}
mbedtls_platform_mutex_t;

typedef struct
{
	void *pCondVar;		// opaque pointer to a condition variable (see threadingmodule.cpp)
}
mbedtls_platform_condition_variable_t;

#endif
