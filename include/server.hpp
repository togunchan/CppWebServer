#ifndef SERVER_HPP
#define SERVER_HPP

#include <iostream>     // for cout, endl
#include <cstdint>      // for uint16_t
#include <cstdlib>      // for exit, EXIT_FAILURE
#include <cstdio>       // for perror
#include <sys/socket.h> // for socket, bind, listen, accept
#include <netinet/in.h> // for sockaddr_in, INADDR_ANY, htons
#include <arpa/inet.h>  // for htonl, ntohl (if needed)
#include <unistd.h>     // for close()
#include <cstring>      // for std::memset
#include <map>          // for std::map
#include <string>       // for std::string

constexpr int BACKLOG = 10; // Maximum number of pending connections
constexpr uint16_t PORT = 8080;
constexpr size_t MAX_REQ_SIZE = 8192;

// A basic HTTP request
struct HttpRequest
{
    std::string method;
    std::string path;
    std::string version;
    std::map<std::string, std::string> headers;
};

// Creates an IPv4 TCP socket, exits on failure
int createTcpSocket();

// Binds given socket FD to the specified port, exits on failure
void bindSocket(int fd, uint16_t port);

// Starts listening on socket FD with backlog = BACKLOG, exits on failure
void startListening(int fd);

// Waits for a client to connect, then echoes back any data received.
// Returns the client socket FD. Exits on accept() failure.
// Blocks until a client connects; returns the new connection's FD
int waitForClient(int server_fd);

// Sends a minimal HTTP/1.1 response.
// 'fd' is the client socket file descriptor, 'body' is the response content.
void sendResponse(int fd, const std::string &body);

// When the headers end, two consecutive "\r\n" sequences appear, forming an empty line. This marks the end of the headers.
// The code continues reading from the socket until this double blank line ("\r\n\r\n") is encountered, ensuring all headers (request line and header lines) are read.
HttpRequest receiveRequest(int fd);

void sendResponse(int fd, const std::string &body, const std::string &contentType = "text/plain");

// Echo loop: read data from client and send it back
void echoLoop(int client_fd);

// Determine the Content-Type header value based on the file extension in `path`.
// Searches for the last '.' in the filename, converts the extension to lowercase,
// and returns a matching MIME type (e.g., ".html" → "text/html", ".css" → "text/css").
// If no known extension is found, defaults to "application/octet-stream".
std::string getMimeType(const std::string &path);

#endif // SERVER_HPP