#ifndef FILESERVER_HPP
#define FILESERVER_HPP

#include <string>
#include <openssl/ssl.h>

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

void serveStaticFileSSL(SSL *ssl, const std::string &path, const std::string &docRoot);

/*
 * Sends a complete HTTP/1.1 response over the socket FD:
 *  - Builds the status line ("HTTP/1.1 200 OK") and headers:
 *      Content-Type, Content-Length, Connection: close
 *  - Appends the message body.
 *  - Writes the full response, looping until all bytes are sent.
 */
void sendResponse(int fd, const std::string &body, const std::string &contentType = "text/plain");

void sendResponseSSL(SSL *ssl, const std::string &body, const std::string &contentType);

/*
 * Write the entire contents of `data` to the socket `fd`.
 * Loops on write() until all bytes are sent or an error occurs.
 */
void sendRaw(int fd, const std::string &data);

void sendRaw(SSL *ssl, const std::string &data);

// return (found, content, mime)
bool peekFile(const std::string &path, const std::string &docRoot, std::string &content, std::string &mime);

#endif // FILESERVER_HPP
