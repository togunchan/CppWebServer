#ifndef SERVER_HPP
#define SERVER_HPP

#include <iostream>     // for cout, endl
#include <cstdint>      // for uint16_t
#include <cstdlib>      // for exit, EXIT_FAILURE
#include <cstdio>       // for perror
#include <sys/socket.h> // for socket, bind, listen, accept
#include <netinet/in.h> // for sockaddr_in, INADDR_ANY, htons
#include <arpa/inet.h>  // for htonl, ntohl (if needed)
#include <unistd.h>     // for close()
#include <cstring>      // for std::memset

// Maximum number of pending connections
constexpr int BACKLOG = 10;
constexpr uint16_t PORT = 8080;

// Creates an IPv4 TCP socket, exits on failure
int createTcpSocket();

// Binds given socket FD to the specified port, exits on failure
void bindSocket(int fd, uint16_t port);

// Starts listening on socket FD with backlog = BACKLOG, exits on failure
void startListening(int fd);

// Blocks until a client connects; returns the new connection's FD
int waitForClient(int server_fd);

// Sends a minimal HTTP/1.1 response.
// 'fd' is the client socket file descriptor, 'body' is the response content.
void sendResponse(int fd, const std::string &body);

#endif // SERVER_HPP