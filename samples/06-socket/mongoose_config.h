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

inline
in_addr_t inet_addr(const char *)
{
    printf("inet_addr() not implemented\n");
    return (in_addr_t)(-1);
}

// With the default of 512 for example requests from Chrome
// apparently fail because the headers are too large.
#ifndef MG_IO_SIZE
#define MG_IO_SIZE 16384
#endif

// Increase the backlog size to the possible maximum to
// avoid errors when many files are fetched in parallel.
#ifndef MG_SOCK_LISTEN_BACKLOG_SIZE
#define MG_SOCK_LISTEN_BACKLOG_SIZE SOMAXCONN
#endif