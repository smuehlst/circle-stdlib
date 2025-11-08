# Posix Socket Support In circle-stdlib

## Overview

circle-stdlib implements the Posix socket interface based on Circle's
socket implementation. The implementation is still in the experimental
stage. If it turns out that real-world applications or libraries
require significant modifications to work with this implementation, it
may be removed again. In that case it makes more sense to use Circle's
socket functions directly.

Due to the bare-metal nature of Circle not all functions and features
are implmented. This document provides nformation what is implemented
of the Posix interface and what is not.

In the experimental stage unimplemented functions will log this fact
and will return an error code. There may also be functions that are
implemented but untested. This fact will also be logged.

### Implemted Functions

#### socket()
#### bind()
#### listen()
#### connect()
#### recv()

`flags` argument is not implemented.

#### send()

`flags` argument is not implemented.

#### recvfrom()

Untested.

#### setsockopt()

Minimal implementation, most options are not implemented.

#### htonl()
#### htons()
#### ntohl()
#### ntohs()

### Unimplemented Functions

#### socketpair()

Not available in Circle, probably no use case in a bare-metal environment anyway.

#### getpeername()
#### getsockname()
#### getsockopt()
#### recvmsg()
#### sendmsg()
#### sendto()
#### shutdown()
