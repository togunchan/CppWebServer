#include "../include/SocketManager.hpp"
#include "../include/Logger.hpp"
#include "../include/ConnectionManager.hpp"
#include <unistd.h>

int main()
{
    int server_fd = createTcpSocket();
    bindSocket(server_fd, PORT);
    startListening(server_fd);

    while (true)
    {
        // Wait for a new client connection
        int client_fd = waitForClient(server_fd);
        spawnClientThread(client_fd);
    }

    close(server_fd);
    return 0;
}