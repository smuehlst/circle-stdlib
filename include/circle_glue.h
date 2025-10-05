#ifndef _circle_glue_h
#define _circle_glue_h

class CConsole;
class CNetSubSystem;

/**
 * Initialize stdio only for console I/O (stdin/stdout/stderr).
 * Must only be called once.
 */
void CGlueStdioInit (CConsole& rConsole);

/**
 * Initialize circle-newlib network subsystem with reference to Circle's
 * network subsystem.
 * Must only be called once.
 */
void CGlueNetworkInit (CNetSubSystem& rNetwork);

#endif
