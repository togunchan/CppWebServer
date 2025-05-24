#include "../include/HttpParser.hpp"
#include <iostream> // for std::cout
#include <fstream>  // for std::ifstream
#include <unistd.h> // for read
#include <sstream>
#include <openssl/ssl.h>

HttpRequest receiveRequest(int fd)
{
    // Reserve space to avoid frequent reallocations
    std::string raw;
    raw.reserve(MAX_REQ_SIZE);
    char buffer[512];
    // Read until "\r\n\r\n"
    while (raw.find("\r\n\r\n") == std::string::npos)
    {
        ssize_t bytes_read = read(fd, buffer, sizeof(buffer));
        if (bytes_read < 0)
        {
            perror("read");
            exit(EXIT_FAILURE);
        }
        else if (bytes_read == 0)
        {
            std::cout << "Client disconnected\n";
            exit(EXIT_FAILURE);
        }
        // Append exactly bytes_read characters from buffer to raw
        raw.append(buffer, bytes_read);
        if (raw.size() > MAX_REQ_SIZE)
        {
            std::cout << "Request too large\n";
            exit(EXIT_FAILURE);
        }
    }

    // Create a stream over the raw string
    std::istringstream stream(raw);
    std::string line;

    // 1) Parse Request-Line: e.g. "GET /index.html HTTP/1.1"
    HttpRequest req;
    if (!std::getline(stream, line) || line.back() != '\r')
    {
        std::cout << "Invalid request\n";
        exit(EXIT_FAILURE);
    }

    {
        std::istringstream rl(line);
        rl >> req.method >> req.path >> req.version;
    }

    // 2) Parse headers until empty line
    while (std::getline(stream, line) && line != "\r" && !line.empty())
    {
        // If the line ends with '\r', remove it
        if (line.back() == '\r')
            line.pop_back();

        // Find the ':' separator between header name and value
        size_t pos = line.find(':');
        if (pos != std::string::npos)
        {
            std::string name = line.substr(0, pos);
            std::string value = line.substr(pos + 1);
            // Trim leading space
            if (!value.empty() && value[0] == ' ')
                value.erase(0, 1);
            req.headers[name] = value;
        }
    }
    return req;
}

HttpRequest receiveRequest(SSL *ssl)
{
    // Reserve space to avoid frequent reallocations
    std::string raw;
    raw.reserve(MAX_REQ_SIZE);
    char buffer[512];
    // Read until "\r\n\r\n"
    while (raw.find("\r\n\r\n") == std::string::npos)
    {
        ssize_t bytes_read = SSL_read(ssl, buffer, sizeof(buffer));
        if (bytes_read < 0)
        {
            perror("read");
            exit(EXIT_FAILURE);
        }
        else if (bytes_read == 0)
        {
            std::cout << "Client disconnected\n";
            exit(EXIT_FAILURE);
        }
        // Append exactly bytes_read characters from buffer to raw
        raw.append(buffer, bytes_read);
        if (raw.size() > MAX_REQ_SIZE)
        {
            std::cout << "Request too large\n";
            exit(EXIT_FAILURE);
        }
    }

    // Create a stream over the raw string
    std::istringstream stream(raw);
    std::string line;

    // 1) Parse Request-Line: e.g. "GET /index.html HTTP/1.1"
    HttpRequest req;
    if (!std::getline(stream, line) || line.back() != '\r')
    {
        std::cout << "Invalid request\n";
        exit(EXIT_FAILURE);
    }

    {
        std::istringstream rl(line);
        rl >> req.method >> req.path >> req.version;
    }

    // 2) Parse headers until empty line
    while (std::getline(stream, line) && line != "\r" && !line.empty())
    {
        // If the line ends with '\r', remove it
        if (line.back() == '\r')
            line.pop_back();

        // Find the ':' separator between header name and value
        size_t pos = line.find(':');
        if (pos != std::string::npos)
        {
            std::string name = line.substr(0, pos);
            std::string value = line.substr(pos + 1);
            // Trim leading space
            if (!value.empty() && value[0] == ' ')
                value.erase(0, 1);
            req.headers[name] = value;
        }
    }
    return req;
}