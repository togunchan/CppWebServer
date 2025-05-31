#include "../include/HttpResponse.hpp"
#include <unistd.h>
#include <sstream>
#include <cstdio>
#include <cstdlib>

void writeAll(int fd, const std::string &data)
{
    const char *ptr = data.c_str();
    ssize_t to_send = data.size();
    ssize_t total_sent = 0;
    while (total_sent < to_send)
    {
        ssize_t sent = write(fd, ptr + total_sent, to_send - total_sent);
        if (sent < 0)
        {
            perror("write");
            exit(EXIT_FAILURE);
        }
        total_sent += sent;
    }
}

void sendErrorResponse(int fd, int status, const std::string &reason, const std::string &body)
{
    std::ostringstream resp;
    resp << "HTTP/1.1 " << status << " " << reason << "\r\n"
         << "Content-Length: " << body.size() << "\r\n"
         << "Connection: close\r\n\r\n"
         << body;
    writeAll(fd, resp.str());
}
