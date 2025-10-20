#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/select.h>
#include <poll.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#include "testkernel.h"

int
main (void)
{
    return CTestKernel::RunTests();
}

TEST_CASE("Basic select read/write test")
{
    constexpr int START_PORT = 5000;
    constexpr size_t NUM_PORTS = 3;
    int socks[NUM_PORTS];
    int maxfd = 0;

    for (auto &sock : socks)
    {
        sock = socket(AF_INET, SOCK_STREAM, 0);
        REQUIRE(sock >= 0);

        // QEMU connects via guestfwd to a simulated server on IP address 10.0.2.2 port 5000
        uint32_t const simulated_server_ip = (10U << 24) | (0U << 16) | (2U << 8) | 2U;

        /* The sin_port and sin_addr members shall be in network byte order. */
        struct sockaddr_in server_address;
        server_address.sin_family = AF_INET;
        server_address.sin_addr.s_addr = htonl(simulated_server_ip);
        server_address.sin_port = htons(START_PORT + (&sock - socks));

        REQUIRE(connect(sock, (struct sockaddr *)&server_address, sizeof(server_address)) >= 0);

        maxfd = std::max(maxfd, sock);
    }

    // Monitor all sockets for writability
    fd_set writefds;
    FD_ZERO(&writefds);
    for (auto const sock : socks)
    {
        FD_SET(sock, &writefds);
    }

    struct timeval timeout = {5, 0}; // 5 second timeout
    int ready = select(maxfd + 1, NULL, &writefds, NULL, &timeout);
    REQUIRE(ready > 0);

    for (auto const &sock : socks)
    {
        if (FD_ISSET(sock, &writefds))
        {
            unsigned int const i = &sock - socks;
            char msg[64];
            snprintf(msg, sizeof(msg), "Hello from client on port %d\n", START_PORT + i);
            ssize_t const sent = write(sock, msg, strlen(msg));
            REQUIRE(sent >= 0);

            printf("Sent to port %d: %s", START_PORT + i, msg);
        }
    }

    // --- Now: Wait for readability and read replies ---
    fd_set readfds;
    FD_ZERO(&readfds);
    for (auto const sock : socks)
    {
        FD_SET(sock, &readfds);
    }

    timeout.tv_sec = 5;
    timeout.tv_usec = 0;
    ready = select(maxfd + 1, &readfds, NULL, NULL, &timeout);
    REQUIRE(ready > 0);
    char buf[128];
    for (auto const sock : socks)
    {
        if (FD_ISSET(sock, &readfds))
        {
            unsigned int const i = &sock - socks;
            ssize_t const n = read(sock, buf, sizeof(buf) - 1);
            REQUIRE(n >= 0);
            if (n > 0)
            {
                buf[n] = '\0';
                printf("Reply from port %d: %s", START_PORT + i, buf);
            }
            else
            {
                printf("Port %d: connection closed\n", START_PORT + i);
            }
        }
    }

    for (auto const &sock : socks)
    {
        REQUIRE(close(sock) >= 0);
    }
}

TEST_CASE("Basic poll read/write test")
{
    constexpr int START_PORT = 5000;
    constexpr size_t NUM_PORTS = 3;
    int socks[NUM_PORTS];

    for (auto &sock : socks)
    {
        sock = socket(AF_INET, SOCK_STREAM, 0);
        REQUIRE(sock >= 0);

        uint32_t const simulated_server_ip = (10U << 24) | (0U << 16) | (2U << 8) | 2U;

        struct sockaddr_in server_address;
        server_address.sin_family = AF_INET;
        server_address.sin_addr.s_addr = htonl(simulated_server_ip);
        server_address.sin_port = htons(START_PORT + (&sock - socks));

        REQUIRE(connect(sock, (struct sockaddr *)&server_address, sizeof(server_address)) >= 0);
    }

    // Monitor all sockets for writability using poll()
    struct pollfd fds[NUM_PORTS];
    for (size_t i = 0; i < NUM_PORTS; ++i)
    {
        fds[i].fd = socks[i];
        fds[i].events = POLLOUT;
        fds[i].revents = 0;
    }

    int ready = poll(fds, NUM_PORTS, 5000); // 5 second timeout
    REQUIRE(ready > 0);

    for (size_t i = 0; i < NUM_PORTS; ++i)
    {
        if (fds[i].revents & POLLOUT)
        {
            char msg[64];
            snprintf(msg, sizeof(msg), "Hello from client on port %d\n", START_PORT + (int)i);
            ssize_t const sent = write(fds[i].fd, msg, strlen(msg));
            REQUIRE(sent >= 0);
            printf("Sent to port %d: %s", START_PORT + (int)i, msg);
        }
    }

    // --- Now: Wait for readability and read replies using poll() ---
    for (size_t i = 0; i < NUM_PORTS; ++i)
    {
        fds[i].fd = socks[i];
        fds[i].events = POLLIN;
        fds[i].revents = 0;
    }

    ready = poll(fds, NUM_PORTS, 5000);
    REQUIRE(ready > 0);

    char buf[128];
    for (size_t i = 0; i < NUM_PORTS; ++i)
    {
        if (fds[i].revents & POLLIN)
        {
            ssize_t const n = read(fds[i].fd, buf, sizeof(buf) - 1);
            REQUIRE(n >= 0);
            if (n > 0)
            {
                buf[n] = '\0';
                printf("Reply from port %d: %s", START_PORT + (int)i, buf);
            }
            else
            {
                printf("Port %d: connection closed\n", START_PORT + (int)i);
            }
        }
    }

    for (auto const &sock : socks)
    {
        REQUIRE(close(sock) >= 0);
    }
}