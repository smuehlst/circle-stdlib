#ifndef _circle_glue_h
#define _circle_glue_h

#include "wrap_fatfs.h"
#include <circle/input/console.h>

/**
 * Initialize stdio for file system and console I/O (stdin/stdout/stderr).
 * Must only be called once.
 */
void CGlueStdioInit (FATFS& rFATFileSystem, CConsole& rConsole);

/**
 * Initialize stdio only for file system I/O.
 * Must only be called once.
 */
void CGlueStdioInit (FATFS& rFATFileSystem);

/**
 * Initialize stdio only for console I/O  (stdin/stdout/stderr).
 * Must only be called once.
 */
void CGlueStdioInit (CConsole& rConsole);

#endif
