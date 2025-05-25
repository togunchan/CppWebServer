#ifndef HTTPPARSER_HPP
#define HTTPPARSER_HPP

#include <map>    // for std::map
#include <string> // for std::string
#include <openssl/ssl.h>

constexpr size_t MAX_REQ_SIZE = 8192;

// A basic HTTP request
struct HttpRequest
{
    std::string method;
    std::string path;
    std::string version;
    std::map<std::string, std::string> headers;
};

/*
 * When the headers end, two consecutive "\r\n" sequences appear, forming an empty line. This marks the end of the headers.
 * The code continues reading from the socket until this double blank line ("\r\n\r\n") is encountered, ensuring all headers (request line and header lines) are read.
 */
HttpRequest receiveRequest(int fd);

HttpRequest receiveRequestSSL(SSL *ssl);
#endif // HTTPPARSER_HPP