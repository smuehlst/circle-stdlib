//
// kernel.cpp
//
// Circle - A C++ bare metal environment for Raspberry Pi
// Copyright (C) 2014-2020  R. Stange <rsta2@o2online.de>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
#include "kernel.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cerrno>
#include <cstdarg>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <vector>
#include <exception>
#include <memory>
#include <dirent.h>
#include <string>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "mongoose.h"

using namespace std;

namespace
{
    const char FromKernel[] = "Smoke Test";

    // Connection event handler function
    void ev_handler(struct mg_connection *c, int ev, void *ev_data)
    {
        if (ev == MG_EV_HTTP_MSG)
        {                                                                   // New HTTP request received
            struct mg_http_message *hm = (struct mg_http_message *)ev_data; // Parsed HTTP request
            if (mg_match(hm->uri, mg_str("/api/hello"), NULL))
            {                                                                // REST API call?
                mg_http_reply(c, 200, "", "{%m:%d}\n", MG_ESC("status"), 1); // Yes. Respond JSON
            }
            else
            {
                struct mg_http_serve_opts opts = {.root_dir = ".", .fs = &mg_fs_posix};
                mg_http_serve_dir(c, hm, &opts); // For all other URLs, Serve static files
            }
        }
    }

}

CKernel::CKernel(void)
    : CStdlibAppStdio("05-smoketest")
{
    mActLED.Blink(5); // show we are alive
}

CStdlibApp::TShutdownMode
CKernel::Run(void)
{
    mLogger.Initialize(&m_LogFile);
    m_Net.Initialize();

    CGlueNetworkInit(m_Net);

    mLogger.Write(GetKernelName(), LogNotice,
                  "Compile time: " __DATE__ " " __TIME__);

    struct mg_mgr mgr; // Mongoose event manager. Holds all connections
    mg_mgr_init(&mgr); // Initialise event manager
    mg_http_listen(&mgr, "http://0.0.0.0:8000", ev_handler, NULL);
    for (;;)
    {
        mg_mgr_poll(&mgr, 1000); // Infinite event loop
    }

    mLogger.Write(GetKernelName(), LogNotice, "Shutting down...");

    return ShutdownHalt;
}
