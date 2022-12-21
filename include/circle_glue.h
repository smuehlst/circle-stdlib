#ifndef _circle_glue_h
#define _circle_glue_h

#include "wrap_fatfs.h"
#include <circle/input/console.h>

/**
 * Initialize stdio only for console I/O (stdin/stdout/stderr).
 * Must only be called once.
 */
void CGlueStdioInit (CConsole& rConsole);

#endif
