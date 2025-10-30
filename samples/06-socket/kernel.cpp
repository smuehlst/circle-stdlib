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

namespace
{
#define SVELTE_BUILD_DIR "circle-socket-app"
#define SVELTE_FALLBACK_FILE SVELTE_BUILD_DIR "/200.html"

    struct mg_http_serve_opts const mg_opts = {.root_dir = SVELTE_BUILD_DIR};
              
    // Connection event handler function
    void ev_handler(struct mg_connection *c, int ev, void *ev_data)
    {
        if (ev == MG_EV_HTTP_MSG)
        {                                                                   // New HTTP request received
            struct mg_http_message *hm = (struct mg_http_message *)ev_data; // Parsed HTTP request
            if (mg_match(hm->uri, mg_str("/api/hello"), NULL))
            {                                                                // REST API call?
                mg_http_reply(c, 200, "", "{%m:%d}\n", MG_ESC("status"), 1); // Yes. Respond JSON
                return;
            }

            // Check whether file exists. If not, server the fallback file
            std::string const uri_path(hm->uri.buf, hm->uri.len);
            std::string file_path = std::string(SVELTE_BUILD_DIR) + uri_path;

            struct stat buffer;
            boolean file_found = false;
            if (stat(file_path.c_str(), &buffer) == 0)
            {
                if (S_ISDIR(buffer.st_mode))
                {
                    if ((file_path.back() != '/'))
                    {
                        file_path += '/';
                    }
                    // If it's a directory, serve index.html
                    file_path += "index.html";
                    file_found = stat(file_path.c_str(), &buffer) == 0
                        && S_ISREG(buffer.st_mode);
                }
                else
                {
                    file_found = S_ISREG(buffer.st_mode);
                }

                if (file_found)
                {
                    // File exists, serve it        
                    mg_http_serve_file(c, hm, file_path.c_str(), &mg_opts);
                    return;
                }
            }
            
            // File does not exist, serve the fallback file.
            // Implement 200.html SPA Fallback for SvelteKit.
            mg_http_serve_file(c, hm, SVELTE_FALLBACK_FILE, &mg_opts);
        }
    }
}

CKernel::CKernel(void)
    : CStdlibAppStdio("06-socket")
{
    mActLED.Blink(5); // show we are alive
}

namespace
{
    struct MongooseLogger
    {
        MongooseLogger(CLogger &logger) : logger(logger) {}
        std::string buffer;
        CLogger &logger;
    };

    extern "C"
    {
        void mongoose_log_fn(char c, void *user_data)
        {
            MongooseLogger *const mg_logger = static_cast<MongooseLogger *>(user_data);

            if (c == '\n')
            {
                mg_logger->logger.Write("Mongoose", LogDebug, "%s", mg_logger->buffer.c_str());
                mg_logger->buffer.clear();
            }
            else
            {
                mg_logger->buffer += c;
            }
        }
    }
}

CStdlibApp::TShutdownMode
CKernel::Run(void)
{
    mLogger.Initialize(&m_LogFile);
    m_Net.Initialize();

    CGlueNetworkInit(m_Net);

    mLogger.Write(GetKernelName(), LogNotice,
                  "Compile time: " __DATE__ " " __TIME__);

    MongooseLogger moongoose_logger(mLogger);
    mg_log_set_fn(mongoose_log_fn, &moongoose_logger);

    // mg_log_set(MG_LL_VERBOSE);

    struct mg_mgr mgr; // Mongoose event manager. Holds all connections
    mg_mgr_init(&mgr); // Initialise event manager
    mg_http_listen(&mgr, "http://0.0.0.0:80", ev_handler, NULL);
    for (;;)
    {
        mg_mgr_poll(&mgr, 1000); // Infinite event loop
    }

    mLogger.Write(GetKernelName(), LogNotice, "Shutting down...");

    return ShutdownHalt;
}
