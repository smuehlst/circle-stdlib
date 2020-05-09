/*
 * wrap_fatfs.h
 *
 * Wrap inclusion of the "fatfs.h" header to redefine the DIR
 * type that collides with the same name from newlib.
 */

#ifndef INCLUDE_WRAP_FATFS_H_
#define INCLUDE_WRAP_FATFS_H_

#define DIR FATFS_DIR
#include <fatfs/ff.h>
#undef DIR

#endif /* INCLUDE_WRAP_FATFS_H_ */
