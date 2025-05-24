#ifndef CONNECTIONMANAGER_HPP
#define CONNECTIONMANAGER_HPP

#include <openssl/ssl.h>
#include <openssl/err.h>

/*
 * Handles a single client connection.
 * Intended to run in its own detached thread.
 *  1. Reads and parses the incoming HTTP request via receiveRequest().
 *  2. Logs the request line and headers.
 *  3. Serves the requested static file or sends an error response.
 *  4. Closes the client socket when finished.
 */
void handleClient(int client_fd);

void handleClient(int *ssl);

/*
 * Spawn a detached thread to handle a client connection.
 *
 * Creates a new std::thread that runs handleClient on the given socket
 * and detaches it so the main loop can immediately continue accepting new clients.
 */
void spawnClientThread(int client_fd);

void spawnClientThread(int client_fd, SSL_CTX *sslCtx);
#endif // CONNECTIONMANAGER_HPP
