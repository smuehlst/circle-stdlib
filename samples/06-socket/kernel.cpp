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
#include <string>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#include <doctest/doctest.h>

#include "doctest_logger.h"

CKernel::CKernel(void)
    : CStdlibAppStdio("06-socket")
{
    mActLED.Blink(5); // show we are alive
}

CStdlibApp::TShutdownMode
CKernel::Run(void)
{
    doctest::Context context;

    mLogger.Initialize(&m_LogFile);
    m_Net.Initialize();

    CGlueNetworkInit(m_Net);

    mLogger.Write(GetKernelName(), LogNotice,
                  "Compile time: " __DATE__ " " __TIME__);

    DoctestLogger::circle_logger = &mLogger;

    // SocketTest();
    int res = context.run(); // run

    mLogger.Write(GetKernelName(), LogNotice, "Shutting down with exit status %d...", res);

    return static_cast<CStdlibApp::TShutdownMode>(res);
}

TEST_CASE("Basic socket() tests")
{
    struct basic_socket_test
    {
        basic_socket_test(int d, int t, int p, bool s) : domain(d), type(t), protocol(p), expect_success(s)
        {
        }

        int domain;
        int type;
        int protocol;
        bool expect_success;
    };

    basic_socket_test const basic_socket_tests[] =
        {
            {AF_INET, SOCK_DGRAM, IPPROTO_UDP, true},
            {AF_INET, SOCK_DGRAM, 0, true},
            {AF_INET, SOCK_STREAM, IPPROTO_TCP, true},
            {AF_INET, SOCK_STREAM, 0, true},
            {AF_INET, SOCK_DGRAM, IPPROTO_IP, false},
            {AF_INET, SOCK_STREAM, IPPROTO_UDP, false},
            {AF_UNIX, SOCK_STREAM, IPPROTO_TCP, false}};

    auto const socket_creation_test = [&](basic_socket_test const &t)
    {
        int const fd = socket(t.domain, t.type, t.protocol);
        unsigned int const testnum = &t - basic_socket_tests;

        MESSAGE("Basic socket test #", std::to_string(testnum));

        if (t.expect_success)
        {
            REQUIRE(fd != -1);
            REQUIRE(close(fd) >= 0);
        }
        else
        {
            REQUIRE(fd == -1);
        }
    };

    for (auto const &t : basic_socket_tests)
    {
        socket_creation_test(t);
    }
}

TEST_CASE("Basic socket lifecycle test")
{
    int fd = socket(AF_INET, SOCK_STREAM, 0);

    REQUIRE(fd != -1);

    struct sockaddr_in server_address;

    /* The sin_port and sin_addr members shall be in network byte order. */
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = htonl(INADDR_ANY);
    server_address.sin_port = htons(5000);

    int const bind_result = bind(fd, reinterpret_cast<struct sockaddr *>(&server_address), sizeof(server_address));

    REQUIRE(bind_result != -1);

    MESSAGE("bind() on file descriptor ", std::to_string(fd), " succeeded");

    int const listen_result = listen(fd, 3);

    REQUIRE(listen_result != -1);

    MESSAGE("listen() on file descriptor ", std::to_string(fd), " succeeded");

    int connected_fd = accept(fd, nullptr, nullptr);

    REQUIRE(connected_fd != -1);

    MESSAGE("accept() on file descriptor ", std::to_string(fd), " returned ", std::to_string(connected_fd));

    char buffer[100];
    size_t const bufsiz = sizeof(buffer);
    auto read_result = read(connected_fd, buffer, bufsiz);
    REQUIRE(read_result >= 0);

    // Terminate string, strip newline if present
    if (read_result == bufsiz)
    {
        read_result -= 1;
    }
    if (read_result > 0 && buffer[read_result - 1] == '\n')
    {
        read_result -= 1;
    }
    buffer[read_result] = 0;

    MESSAGE("Read from socket succeeded: '", std::string(buffer), "'");

    strcpy(buffer, "Response from server\n");
    auto const write_result = write(connected_fd, buffer, strlen(buffer));
    REQUIRE(write_result >= 0);

    MESSAGE("Write of ", std::to_string(static_cast<int>(write_result)), " bytes to socket succeeded");

    REQUIRE(close(connected_fd) >= 0);

    REQUIRE(close(fd) >= 0);

    fd = socket(AF_INET, SOCK_STREAM, 0);

    REQUIRE(fd != -1);

    // QEMU connects via guestfwd to a simulated server on IP address 10.0.2.100 port 5001
    uint32_t const simulated_server_ip = (10U << 24) | (0U << 16) | (2U << 8) | 100U;
    
    /* The sin_port and sin_addr members shall be in network byte order. */
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = htonl(simulated_server_ip);
    server_address.sin_port = htons(5001);

    int const connect_result =
        connect(fd, reinterpret_cast<struct sockaddr *>(&server_address), sizeof(server_address));
    
    REQUIRE(connect_result >= 0);

    read_result = read(fd, buffer, sizeof(buffer));
    REQUIRE(read_result >= 0);

    MESSAGE("Read ", std::to_string(read_result), " bytes from server");

    char const expected_data[] = "simulated server";
    REQUIRE(strncmp(buffer, expected_data, sizeof(expected_data) - 1) == 0);

    MESSAGE("Received expected data from server");

    REQUIRE(close(fd) >= 0);

    MESSAGE("Read from client connection successful");
}
