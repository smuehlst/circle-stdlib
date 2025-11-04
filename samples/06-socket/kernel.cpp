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

#include <string>
#include <sys/stat.h>
#include "mongoose.h"
#include "nlohmann/json.hpp"
#include <circle/sched/mutex.h>
#include <set>
#include <queue>

namespace
{
    // Connect buttons between these GPIOs and GND
    constexpr int GPIO_BUTTON_17 = 17;
    constexpr int GPIO_BUTTON_18 = 18;

    class MutexHolder
    {
    public:
        MutexHolder(CMutex &mutex) : mutex_ref(mutex)
        {
            mutex_ref.Acquire();
        }

        ~MutexHolder()
        {
            mutex_ref.Release();
        }

    private:
        CMutex &mutex_ref;
    };

    using connection_set = std::set<struct mg_connection *>;
    using message_queue = std::queue<unsigned int>;

    // Set of active WebSocket connections
    connection_set ws_connections;

    // Queue of messages to be sent to WebSocket clients
    message_queue msg_queue;
    CMutex message_queue_mutex;

#define SVELTE_BUILD_DIR "circle-socket-app"
#define SVELTE_FALLBACK_FILE SVELTE_BUILD_DIR "/200.html"

    struct mg_http_serve_opts const mg_opts = {.root_dir = SVELTE_BUILD_DIR};

    void api_status(struct mg_connection *c, struct mg_http_message *hm)
    {
        CKernelOptions const &options = static_cast<CKernel *>(c->fn_data)->GetOptions();
        CCPUThrottle const &cpu = static_cast<CKernel *>(c->fn_data)->GetCPUThrottle();

        nlohmann::json status_json;
        status_json["cpuSpeed"] = static_cast<int>(cpu.GetClockRate());
        status_json["socMaxTemp"] = options.GetSoCMaxTemp();
        status_json["socCurrentTemp"] = cpu.GetTemperature();
        status_json["gpioFanPin"] = options.GetGPIOFanPin();

        std::string const s = status_json.dump();
        mg_http_reply(c, 200, "", "%s\n", s.c_str());
    }

    void http_handler(struct mg_connection *c, struct mg_http_message *hm)
    {
        if (mg_match(hm->uri, mg_str("/ws"), NULL))
        {
            // WebSocket request. Insert into the set of active
            // WebSocket connections.
            mg_ws_upgrade(c, hm, NULL);
            ws_connections.insert(c);
            return;
        }

        if (mg_match(hm->uri, mg_str("/api/status"), NULL)) // REST API call?
        {
            api_status(c, hm);
            return;
        }

        // Check whether file exists. If not, serve the fallback file.
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
                file_found = stat(file_path.c_str(), &buffer) == 0 && S_ISREG(buffer.st_mode);
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

    void websocket_handler(struct mg_connection *c, struct mg_ws_message *wm)
    {
        // Echo message back
        mg_ws_send(c, wm->data.buf, wm->data.len, WEBSOCKET_OP_TEXT);
    }

    // Connection event handler function
    void ev_handler(struct mg_connection *c, int ev, void *ev_data)
    {
        switch (ev)
        {
        case MG_EV_ACCEPT:
            break;
        case MG_EV_HTTP_MSG:
            http_handler(c, static_cast<struct mg_http_message *>(ev_data));
            break;
        case MG_EV_WS_MSG:
            websocket_handler(c, static_cast<struct mg_ws_message *>(ev_data));
            break;
        case MG_EV_CLOSE:
            // Remove from ws_connections if present. If the pointer is not
            // present, in the set this was not a WebSocket connection.
            ws_connections.erase(c);
            break;
        }
    }
}

CKernel::CKernel(void)
    : CStdlibAppStdio("06-socket"),
      m_CPUThrottle (CPUSpeedMaximum),
      m_GPIOManager(CInterruptSystem::Get())
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
#ifdef CIRCLE_QEMU
    mLogger.Initialize(&m_LogFile);
#endif
    m_Net.Initialize();
    m_GPIOManager.Initialize();

    CGlueNetworkInit(m_Net);

    mLogger.Write(GetKernelName(), LogNotice,
                  "Compile time: " __DATE__ " " __TIME__);

    // Start the button monitoring tasks.
    // They will never terminate in this example.
    new CButtonTask(GPIO_BUTTON_17, &m_GPIOManager,
                    ButtonPressedHandler, this);
    new CButtonTask(GPIO_BUTTON_18, &m_GPIOManager,
                    ButtonPressedHandler, this);

    MongooseLogger moongoose_logger(mLogger);
    mg_log_set_fn(mongoose_log_fn, &moongoose_logger);

    // Uncomment for verbose Mongoose logging:
    // mg_log_set(MG_LL_VERBOSE);

    struct mg_mgr mgr; // Mongoose event manager. Holds all connections
    mg_mgr_init(&mgr); // Initialise event manager
    mg_http_listen(&mgr, "http://0.0.0.0:80", ev_handler, this);
    for (;;)
    {
        mg_mgr_poll(&mgr, 1000);

        // Send button press messages to all connected WebSocket clients.
        MutexHolder const mutex_holder(message_queue_mutex);
        while (!msg_queue.empty())
        {
            unsigned int const gpio_pin = msg_queue.front();
            msg_queue.pop();

            std::string const message = "Button pressed on GPIO pin " + std::to_string(gpio_pin);

            for (auto const conn : ws_connections)
            {
                mg_ws_send(conn, message.c_str(), message.length(), WEBSOCKET_OP_TEXT);
            }
        }

        m_CPUThrottle.Update ();
    }

    mLogger.Write(GetKernelName(), LogNotice, "Shutting down...");

    return ShutdownHalt;
}

void CKernel::ButtonPressedHandler(unsigned nGPIOPin, void * /* pParam */)
{
    MutexHolder const mutex_holder(message_queue_mutex);
    msg_queue.push(nGPIOPin);
}
