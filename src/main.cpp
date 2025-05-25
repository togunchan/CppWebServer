#include "../include/SocketManager.hpp"
#include "../include/Logger.hpp"
#include "../include/ConnectionManager.hpp"
#include "../include/Config.hpp"
#include "../include/SSLManager.hpp"
#include <unistd.h>
#include <iostream>

int main()
{

    // HTTP
    Config cfg;
    try
    {
        cfg = Config::load("../config.json");
        log("config port: " + std::to_string(cfg.port));
        log("config sslPort: " + std::to_string(cfg.sslPort));
        log("config docRoot: " + cfg.docRoot);
        log("config maxThreads: " + std::to_string(cfg.maxThreads));
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
    }

    int http_fd = createTcpSocket();
    bindSocket(http_fd, cfg.port);
    startListening(http_fd, cfg.port);

    // HTTPS
    SSL_CTX *sslCtx = createServerSSLContext("../server.crt", "../server.key");
    int https_fd = createTcpSocket();
    bindSocket(https_fd, cfg.sslPort);
    startListening(https_fd, cfg.sslPort);

    while (true)
    {
        // Wait for a new client connection
        int client_fd = waitForClient(http_fd);
        spawnClientThread(client_fd, cfg.docRoot);

        int ssl_client_fd = waitForClient(https_fd);
        spawnClientThread(ssl_client_fd, sslCtx);
    }

    SSL_CTX_free(sslCtx);
    close(http_fd);
    close(https_fd);
    return 0;
}