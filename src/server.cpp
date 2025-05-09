#include "../include/server.hpp"

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

void bindSocket(int fd, uint16_t port)
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

void startListening(int fd)
{
    if (listen(fd, BACKLOG) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }
    std::cout << "Server listening on port 8080" << std::endl;
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
    return client_fd;
}

void sendResponse(int fd, const std::string &body)
{
    std::cout << "I am in sendResponse function. fd is " << fd << std::endl;
    // Build status line and headers
    std::string response =
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/plain\r\n"
        "Content-Length: " +
        std::to_string(body.size()) + "\r\n"
                                      "Connection: close\r\n"
                                      "\r\n" +
        body;

    // Send the full response
    ssize_t total_sent = 0;
    const char *data = response.c_str();
    ssize_t to_send = response.size();
    while (total_sent < to_send)
    {
        ssize_t sent = write(fd, data + total_sent, to_send - total_sent);
        if (sent < 0)
        {
            perror("write");
            exit(EXIT_FAILURE);
        }
        total_sent += sent;
    }
}

int main()
{
    int server_fd = createTcpSocket();
    bindSocket(server_fd, PORT);
    startListening(server_fd);
    int client_fd = waitForClient(server_fd);
    sendResponse(client_fd, "Hello from C++ HTTP Server\n");
    close(server_fd);

    return 0;
}