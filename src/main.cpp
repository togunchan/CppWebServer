#include "../include/SocketManager.hpp"
#include "../include/Logger.hpp"
#include "../include/ConnectionManager.hpp"
#include "../include/Config.hpp"
#include <unistd.h>
#include <iostream>

int main()
{
    Config cfg;
    try
    {
        cfg = Config::load("../config.json");
        log("config port: " + std::to_string(cfg.port));
        log("config docRoot: " + cfg.docRoot);
        log("config maxThreads: " + std::to_string(cfg.maxThreads));
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
    }

    int server_fd = createTcpSocket();
    bindSocket(server_fd, cfg.port);
    startListening(server_fd, cfg.port);

    while (true)
    {
        // Wait for a new client connection
        int client_fd = waitForClient(server_fd);
        spawnClientThread(client_fd);
    }

    close(server_fd);
    return 0;
}