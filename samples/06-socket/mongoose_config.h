#include <stdint.h>
#include <stdbool.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>

#define MG_ENABLE_POSIX_FS 1

// Stubs for unimplemented functions:
#define MG_CUSTOM_NONBLOCK(fd) do { printf("Ignoring nonblock for fd %d\n", fd); } while (0)
// #define inet_addr(x) ((in_addr_t)(-1))

inline
in_addr_t inet_addr(const char *cp)
{
    printf("inet_addr() not implemented\n");
    return (in_addr_t)(-1);
}

#ifndef MG_IO_SIZE
#define MG_IO_SIZE 16384
#endif
