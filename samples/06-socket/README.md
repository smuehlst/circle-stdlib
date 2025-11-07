# Socket API Sample

## Overview

This sample program demonstrates the use of the socket API.
The Mongoose web server serves a web application implemented with
SvelteKit.

## Functionality

### REST API Demo

The "REST API" page uses a REST API URL to fetch a JSON object
containing some status infos, and displays the information.

### WebSocket Demo

On the "WebSocket" page a chat window is shown. Multiple chats
in different browser windows can be opened in parallel. Any message 
typed into the text field at the bottom is echoed back from the server
and broadcasted to all other open chat windows.

In addition to that the server monitors GPIO pins 17 and 18. Anytime
one of these pins is connected to ground via a button, a message is
sent to all active chats.

## Running The Demo

Prepare an SD Card with the standard Circle procedure, using the
kernel image from the 06-sample build. Extract the tar file
[06-socket-webapp.tar.gz](../../testdata/06-socket-webapp.tar.gz)
to the root directory of the SD card. This will create the
directory `circle-socket-app` in the root directory of the SD
card.

When the application is started, it will log its IP address
to the screen. Open a browser and enter the IP address to
open the web application.

## Rebuilding The SvelteKit Web Application

The web application is implemented with the SvelteKit framework:

https://svelte.dev/

Download Node.js from https://nodejs.org/en/download and install it.
Then go to the `circle-socket-app` subdirectory and run the
following commands:

```
npm install
npm run build
```

The web application is generated into the `build` subdirectory.
This directory must be copied as `circle-socket-app` into the
root directory of the SD card.