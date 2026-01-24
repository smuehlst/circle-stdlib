# Posix Socket Support In circle-stdlib

## Overview

circle-stdlib implements the Posix socket interface based on Circle's
socket implementation. The implementation is still in the experimental
stage. If it turns out that real-world applications or libraries
require significant modifications to work with this implementation, it
may be removed again. In that case it makes more sense to use Circle's
socket functions directly.

Due to the bare-metal nature of Circle not all functions and features
are implemented. This document provides information what is implemented
of the Posix interface and what is not.

In the experimental stage unimplemented functions will log this fact
and will return an error code. There may also be functions that are
implemented but untested. This fact will also be logged.

The features that are implemented are sufficient to run a
web server with HTTP and websocket support. The
[06-socket](../samples/06-socket/) sample demonstrates a
small circle-stdlib web application implemented with the
[Mongoose](https://mongoose.ws/) web server library.

## Implemented Protocols

At the moment only TCP sockets are implemented.

UDP socket support is not implemented currently because Circle's
implementation of UDP sockets differs significantly from the
Posix model. UDP support may be added at a later time.

## Implemented Functions

### socket()
### bind()
### listen()
### connect()
### recv()

`flags` argument is not implemented.

### send()

`flags` argument is not implemented.

### recvfrom()

### sendto()

### setsockopt()

Minimal implementation, most options are not implemented.

### htonl()
### htons()
### ntohl()
### ntohs()

## Unimplemented Functions

### socketpair()

Not available in Circle, probably no use case in a bare-metal environment anyway.

### getpeername()
### getsockname()
### getsockopt()
### recvmsg()
### sendmsg()

### shutdown()
