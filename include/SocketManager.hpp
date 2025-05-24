#ifndef SOCKETMANAGER_HPP
#define SOCKETMANAGER_HPP

#include <cstdint> // for uint16_t

constexpr int BACKLOG = 10; // Maximum number of pending connections
// constexpr uint16_t PORT = 8080;

/*
 * Creates an IPv4 TCP socket.
 * Exits on failure.
 */
int createTcpSocket();

/*
 * Binds the given socket FD to the specified port.
 * Exits on failure.
 */
void bindSocket(int fd, uint16_t port);

/*
 * Starts listening on the socket FD with backlog = BACKLOG.
 * Exits on failure.
 */
void startListening(int fd, int port);

/*
 * Waits for a client to connect (blocks until a connection arrives).
 * Returns the client socket FD.
 * Exits on accept() failure.
 */
int waitForClient(int server_fd);

/*
 * Reads data from the client socket in a loop and echoes it back.
 */
void echoLoop(int client_fd);

#endif // SOCKETMANAGER_HPP