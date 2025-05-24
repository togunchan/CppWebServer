#include "../include/SocketManager.hpp"
#include "../include/Logger.hpp"
#include "../include/ConnectionManager.hpp"
#include "../include/Config.hpp"
#include "../include/SSLManager.hpp"
#include <unistd.h>
#include <iostream>

int main()
{

    SSL_CTX *sslCtx = createServerSSLContext("../server.crt", "../server.key");
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

    int server_fd = createTcpSocket();
    bindSocket(server_fd, cfg.sslPort);
    startListening(server_fd, cfg.sslPort);

    while (true)
    {
        // Wait for a new client connection
        // int client_fd = waitForClient(server_fd);
        // spawnClientThread(client_fd);

        int ssl_client_fd = waitForClient(server_fd);
        spawnClientThread(ssl_client_fd, sslCtx);
    }

    close(server_fd);
    return 0;
}