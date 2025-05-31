#include "../include/SocketManager.hpp"
#include "../include/Logger.hpp"
#include "../include/ConnectionManager.hpp"
#include "../include/Config.hpp"
#include "../include/SSLManager.hpp"
#include <unistd.h>
#include <iostream>
#include <thread>
#include <stop_token>

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

    std::thread httpThread([http_fd, &cfg]()
                           {
            while (true)
            {
                log("I am in the http thread.");
                // Wait for a new client connection
                int client_fd = waitForClient(http_fd);
                if(client_fd < 0) continue;
                spawnClientThreadHTTP(client_fd, cfg.docRoot);
            } });

    std::thread httpsThread([https_fd, sslCtx, &cfg]()
                            {
            while (true)
            {
                log("I am in the https thread.");
                int ssl_client_fd = waitForClient(https_fd);
                if(ssl_client_fd < 0) continue;
                spawnClientThreadHTTPS(ssl_client_fd, sslCtx, cfg.docRoot);
            } });

    if (httpThread.joinable())
        httpThread.join();
    if (httpsThread.joinable())
        httpsThread.join();

    SSL_CTX_free(sslCtx);
    close(http_fd);
    close(https_fd);
    return 0;
}