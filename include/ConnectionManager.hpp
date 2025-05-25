#ifndef CONNECTIONMANAGER_HPP
#define CONNECTIONMANAGER_HPP

#include "../include/Config.hpp"
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
void handleClient(int client_fd, const std::string docRoot);

void handleClientSSL(int *ssl, const std::string docRoot);

/*
 * Spawn a detached thread to handle a client connection.
 *
 * Creates a new std::thread that runs handleClient on the given socket
 * and detaches it so the main loop can immediately continue accepting new clients.
 */
void spawnClientThreadHTTP(int client_fd, const std::string &docRoot);

void spawnClientThreadHTTPS(int client_fd, SSL_CTX *sslCtx, const std::string &docRoot);
#endif // CONNECTIONMANAGER_HPP
