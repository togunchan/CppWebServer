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

/*
 * Creates an IPv4 TCP socket.
 * Exits on failure.
 */
int createTcpSocket();

/*
 * Binds the given socket FD to the specified port.
 * Exits on failure.
 */
void bindSocket(int fd, uint16_t port);

/*
 * Starts listening on the socket FD with backlog = BACKLOG.
 * Exits on failure.
 */
void startListening(int fd);

/*
 * Waits for a client to connect (blocks until a connection arrives).
 * Returns the client socket FD.
 * Exits on accept() failure.
 */
int waitForClient(int server_fd);

/*
 * When the headers end, two consecutive "\r\n" sequences appear, forming an empty line. This marks the end of the headers.
 * The code continues reading from the socket until this double blank line ("\r\n\r\n") is encountered, ensuring all headers (request line and header lines) are read.
 */
HttpRequest receiveRequest(int fd);

/*
 * Sends a complete HTTP/1.1 response over the socket FD:
 *  - Builds the status line ("HTTP/1.1 200 OK") and headers:
 *      Content-Type, Content-Length, Connection: close
 *  - Appends the message body.
 *  - Writes the full response, looping until all bytes are sent.
 */
void sendResponse(int fd, const std::string &body, const std::string &contentType = "text/plain");

/*
 * Reads data from the client socket in a loop and echoes it back.
 */
void echoLoop(int client_fd);

/*
 * Determines the Content-Type header value based on the file extension in `path`.
 *  - Finds the last '.' in the filename.
 *  - Converts the extension to lowercase.
 *  - Looks up and returns a matching MIME type (e.g., "html" → "text/html").
 *  - Defaults to "application/octet-stream" if the extension is unknown.
 */
std::string getMimeType(const std::string &path);

/*
 * Serves a static file over HTTP:
 *  - Constructs the full file path using the document root and request path.
 *  - If the path is "/", defaults to serving "index.html".
 *  - Opens the file in binary mode; on failure sends a 404 response.
 *  - Reads the entire file into memory, determines its MIME type,
 *    and sends it using sendResponse().
 */
void serveStaticFile(int fd, const std::string &path, const std::string &docRoot);

/*
 * Thread-safe logging function.
 * Uses a mutex to ensure only one thread at a time writes to std::cout,
 * preventing interleaved or garbled output.
 */
void log(const std::string &message);

/*
 * Handles a single client connection.
 * Intended to run in its own detached thread.
 *  1. Reads and parses the incoming HTTP request via receiveRequest().
 *  2. Logs the request line and headers.
 *  3. Serves the requested static file or sends an error response.
 *  4. Closes the client socket when finished.
 */
void handleClient(int client_fd);

/*
 * Spawn a detached thread to handle a client connection.
 *
 * Creates a new std::thread that runs handleClient on the given socket
 * and detaches it so the main loop can immediately continue accepting new clients.
 */
void spawnClientThread(int client_fd);

#endif // SERVER_HPP