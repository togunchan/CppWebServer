#include "../include/SocketManager.hpp"
#include "../include/Logger.hpp"
#include <cstring>      // for std::memset
#include <cstdio>       // for perror
#include <sys/socket.h> // for socket, bind, listen, accept
#include <netinet/in.h> // for sockaddr_in, INADDR_ANY, htons
#include <arpa/inet.h>  // for htonl, ntohl (if needed)
#include <cstdlib>      // for exit, EXIT_FAILURE
#include <string>       // for std::string
#include <iostream>     // for std::cout
#include <unistd.h>
#include "../include/Config.hpp"

Config cfg;

int createTcpSocket()
{
    // create TCP socket
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0)
    {
        perror("socket");
        return 1;
    }
    return server_fd;
}

void bindSocket(int fd, uint16_t port = cfg.port)
{
    // bind to port 8080 on any interface
    sockaddr_in addr{}; // Declare and zero-initialize the IPv4 address structure
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = INADDR_ANY;
    if (bind(fd, reinterpret_cast<sockaddr *>(&addr), sizeof(addr)) < 0)
    {
        perror("bind");
        exit(EXIT_FAILURE);
    }
}

void startListening(int fd, int port)
{
    if (listen(fd, BACKLOG) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }
    log("Server listening on port " + std::to_string(port) + "\n");
}

int waitForClient(int server_fd)
{
    int client_fd = accept(server_fd, nullptr, nullptr);
    if (client_fd >= 0)
    {
        // const char *msg = "Hello World\n";
        // send(client_fd, msg, strlen(msg), 0);
        //  close(client_fd);
        std::cout << "New client connected: fd= " << client_fd << "\n";
    }
    else
    {
        perror("accept");
        exit(EXIT_FAILURE);
    }

    // close(client_fd);
    return client_fd;
}

void echoLoop(int client_fd)
{
    std::cout << "I am in echoLoop function" << std::endl;
    constexpr size_t BUFFER_SIZE = 1024;
    char buffer[BUFFER_SIZE];
    while (true)
    {
        // Zero out the buffer before reading
        std::memset(buffer, 0, BUFFER_SIZE);
        ssize_t bytes_read = read(client_fd, buffer, BUFFER_SIZE - 1);
        if (bytes_read < 0)
        {
            perror("read");
            break;
        }
        else if (bytes_read == 0)
        {
            std::cout << "Client fd= " << client_fd << "disconnected\n";
            break;
        }

        // Send back exactly what we receive
        size_t total_sent = 0;
        while (total_sent < bytes_read)
        {
            ssize_t sent = write(client_fd, buffer + total_sent, bytes_read - total_sent);
            if (sent < 0)
            {
                perror("write");
                break;
            }
            total_sent += sent;
        }
    }
}