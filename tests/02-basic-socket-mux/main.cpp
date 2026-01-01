#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/select.h>
#include <poll.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <array>
#include <regex>
#include <string>

#include "testkernel.h"

int
main (void)
{
    // CTestKernel::RunTests() does not actually return, but exits or halts the system.
    return CTestKernel::RunTests("02-basic-socket-mux");
}

constexpr int START_PORT = 5000;
constexpr size_t NUM_PORTS = 3;
using SocketArray = std::array<int, NUM_PORTS>;
struct Sockets
{
    SocketArray sockets;
    int maxfd;

    Sockets(): sockets{}, maxfd(0) {}
};

namespace {
    Sockets setup_sockets()
    {
        Sockets sockets;

        // QEMU connects via guestfwd to a simulated server on IP address 10.0.2.2 port 5000
        constexpr uint32_t simulated_server_ip = (10U << 24) | (0U << 16) | (2U << 8) | 2U;

        for (auto &sock : sockets.sockets)
        {
            sock = socket(AF_INET, SOCK_STREAM, 0);
            REQUIRE(sock >= 0);

            /* The sin_port and sin_addr members shall be in network byte order. */
            struct sockaddr_in server_address;
            server_address.sin_family = AF_INET;
            server_address.sin_addr.s_addr = htonl(simulated_server_ip);
            server_address.sin_port = htons(START_PORT + (&sock - sockets.sockets.data()));

            int const server_socket = connect(sock, (struct sockaddr *)&server_address, sizeof(server_address));
            REQUIRE(server_socket >= 0);

            sockets.maxfd = std::max(sockets.maxfd, sock);
        }

        return sockets;
    }
}

TEST_CASE("Basic select read/write test")
{
    Sockets sockets = setup_sockets();

    // Monitor all sockets for writability
    fd_set writefds;
    FD_ZERO(&writefds);
    for (auto const sock : sockets.sockets)
    {
        FD_SET(sock, &writefds);
    }

    struct timeval timeout = {5, 0}; // 5 second timeout
    int ready = select(sockets.maxfd + 1, NULL, &writefds, NULL, &timeout);
    REQUIRE(ready > 0);

    for (auto const &sock : sockets.sockets)
    {
        if (FD_ISSET(sock, &writefds))
        {
            unsigned int const i = &sock - sockets.sockets.data();
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
    for (auto const sock : sockets.sockets)
    {
        FD_SET(sock, &readfds);
    }

    timeout.tv_sec = 5;
    timeout.tv_usec = 0;
    ready = select(sockets.maxfd + 1, &readfds, NULL, NULL, &timeout);
    REQUIRE(ready > 0);
    char buf[128];
    for (auto const sock : sockets.sockets)
    {
        if (FD_ISSET(sock, &readfds))
        {
            unsigned int const i = &sock - sockets.sockets.data();
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

    for (auto const sock : sockets.sockets)
    {
        REQUIRE(close(sock) >= 0);
    }
}

TEST_CASE("Basic sendto()/recvfrom() read/write test")
{
    Sockets sockets = setup_sockets();

    // Test sendto() and recvfrom() in TCP mode (should ignore address parameters)
    constexpr char test_msg[] = "Test message via sendto/recvfrom\n";
    constexpr char test_reply[] = "ACK from port"; // beginning of expected reply

    for (auto const sock : sockets.sockets)
    {
        ssize_t const sent = sendto(sock, test_msg, sizeof(test_msg) - 1, 0, nullptr, 0);
        REQUIRE(sent == sizeof(test_msg) - 1);
    }

    for (auto const sock : sockets.sockets)
    {
        char recv_buf[128];
        ssize_t const recvd = recvfrom(sock, recv_buf, sizeof(recv_buf) - 1, 0, nullptr, nullptr);
        REQUIRE(recvd == sizeof(test_msg) - 1);
        recv_buf[recvd] = '\0';

        // Verify that received message begins with expected reply.
        REQUIRE(std::string(recv_buf).find(test_reply) == 0);

#if 0
        // This would be a more thorough check, but the default kernel size
        // is exceeded when std::regex is used.
        // Match the reply against the
        // reply generated in the testserver.py script with a
        // regular expression.
        // Extract the socket number and the size of the received message
        // and check them against the expected socket number and the test_msg size.

        unsigned int const i = &sock - sockets.sockets.data();
        int const expected_port = START_PORT + i;
        size_t const expected_size = sizeof(test_msg) - 1;

        std::string const reply(recv_buf);
        std::regex const pattern("ACK from port (\\d+): got (\\d+) bytes\\n");
        std::smatch matches;
        REQUIRE(std::regex_match(reply, matches, pattern));

        int const port = std::stoi(matches[1].str());
        int const size = std::stoi(matches[2].str());
        REQUIRE(port == expected_port);
        REQUIRE(size == expected_size);
#endif
    }

    for (auto const sock : sockets.sockets)
    {
        REQUIRE(close(sock) >= 0);
    }
}

TEST_CASE("Basic poll read/write test")
{
    Sockets sockets = setup_sockets();

    // Monitor all sockets for writability using poll()
    struct pollfd fds[NUM_PORTS];
    for (size_t i = 0; i < NUM_PORTS; ++i)
    {
        fds[i].fd = sockets.sockets[i];
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
        fds[i].fd = sockets.sockets[i];
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

    for (auto const sock : sockets.sockets)
    {
        REQUIRE(close(sock) >= 0);
    }
}