#include "../include/FileServer.hpp"
#include "../include/Logger.hpp"
#include <unordered_map>
#include <algorithm>
#include <iterator>
#include <fstream>
#include <sstream>
#include <string>
#include <unistd.h>
#include <openssl/ssl.h>

std::string getMimeType(const std::string &path)
{
    // Find last dot in filename
    size_t pos = path.rfind('.');
    if (pos == std::string::npos)
        return "application/octet-stream";

    // find extention
    std::string ext = path.substr(pos + 1);
    // make lower case
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

    // static so that the lookup table is constructed only once and shared across calls
    static const std::unordered_map<std::string, std::string> mimeTypes = {
        {"html", "text/html"},
        {"htm", "text/html"},
        {"css", "text/css"},
        {"js", "application/javascript"},
        {"json", "application/json"},
        {"txt", "text/plain"},
        {"png", "image/png"},
        {"jpg", "image/jpeg"},
        {"jpeg", "image/jpeg"},
        {"gif", "image/gif"},
        {"ico", "image/x-icon"},
        {"svg", "image/svg+xml"},
        {"pdf", "application/pdf"},
        {"doc", "application/msword"}};

    std::unordered_map<std::string, std::string>::const_iterator it = mimeTypes.find(ext);
    // if type not found
    if (it == mimeTypes.end())
        return "application/octet-stream";
    return it->second;
}

void serveStaticFile(int fd, const std::string &path, const std::string &docRoot)
{
    std::string fullPath = docRoot + path;
    log("Serving file: " + fullPath);
    if (path == "/")
        fullPath += "index.html";

    // input file stream
    // In text mode, some systems translate end-of-line sequences ("\r\n" <-> "\n");
    // in binary mode, no such translations occur and bytes are read exactly as-is.
    std::ifstream file(fullPath, std::ios::binary);
    if (!file)
    {
        sendResponse(fd, "404 Not Found\n", "text/plain");
        log("fullPath is " + fullPath);
        return;
    }

    // Read entire file into a string using input stream buffer iterators
    std::string content(
        (std::istreambuf_iterator<char>(file)), // iterator positioned at start of file stream
        std::istreambuf_iterator<char>()        // end-of-stream iterator indicating EOF(end of file)
    );

    std::string mime = getMimeType(fullPath);
    sendResponse(fd, content, mime);
}

void serveStaticFileSSL(SSL *ssl, const std::string &path, const std::string &docRoot)
{
    std::string fullPath = docRoot + path;
    log("Serving file: " + fullPath);
    if (path == "/")
        fullPath += "index.html";

    // input file stream
    // In text mode, some systems translate end-of-line sequences ("\r\n" <-> "\n");
    // in binary mode, no such translations occur and bytes are read exactly as-is.
    std::ifstream file(fullPath, std::ios::binary);
    if (!file)
    {
        sendResponseSSL(ssl, "404 Not Found\n", "text/plain");
        log("fullPath is " + fullPath);
        return;
    }

    // Read entire file into a string using input stream buffer iterators
    std::string content(
        (std::istreambuf_iterator<char>(file)), // iterator positioned at start of file stream
        std::istreambuf_iterator<char>()        // end-of-stream iterator indicating EOF(end of file)
    );

    std::string mime = getMimeType(fullPath);
    sendResponseSSL(ssl, content, mime);
}

void sendResponse(int fd, const std::string &body, const std::string &contentType)
{
    // Build status line and headers dynamically
    std::ostringstream resp;
    resp << "HTTP/1.1 200 OK\r\n"
         << "Content-Type: " << contentType << "\r\n"
         << "Content-Length: " << body.size() << "\r\n"
         << "Connection: close\r\n\r\n"
         << body;

    // Send the full response
    std::string response = resp.str();
    const char *data = response.c_str();
    ssize_t to_send = response.size();
    ssize_t total_sent = 0;
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

void sendResponseSSL(SSL *ssl, const std::string &body, const std::string &contentType)
{
    // Build status line and headers dynamically
    std::ostringstream resp;
    resp << "HTTP/1.1 200 OK\r\n"
         << "Content-Type: " << contentType << "\r\n"
         << "Content-Length: " << body.size() << "\r\n"
         << "Connection: close\r\n\r\n"
         << body;

    // Send the full response
    std::string response = resp.str();
    const char *data = response.c_str();
    ssize_t to_send = response.size();
    ssize_t total_sent = 0;
    while (total_sent < to_send)
    {
        ssize_t sent = SSL_write(ssl, data + total_sent, to_send - total_sent);
        if (sent < 0)
        {
            perror("write");
            exit(EXIT_FAILURE);
        }
        total_sent += sent;
    }
}

void sendRaw(int fd, const std::string &data)
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

void sendRaw(SSL *ssl, const std::string &data)
{
    const char *ptr = data.c_str();
    ssize_t to_send = data.size();
    ssize_t total_sent = 0;
    while (total_sent < to_send)
    {
        ssize_t sent = SSL_write(ssl, ptr + total_sent, to_send - total_sent);
        if (sent < 0)
        {
            perror("write");
            exit(EXIT_FAILURE);
        }
        total_sent += sent;
    }
}

bool peekFile(const std::string &path, const std::string &docRoot, std::string &content, std::string &mime)
{
    std::string fullPath = docRoot + path;
    log("Peeking file: " + fullPath);
    if (path == "/")
        fullPath += "index.html";

    std::ifstream file(fullPath, std::ios::binary);
    if (!file)
    {
        log("peekFile returned false");
        return false;
    }

    content.assign(
        (std::istreambuf_iterator<char>(file)), // iterator positioned at start of file stream
        std::istreambuf_iterator<char>()        // end-of-stream iterator indicating EOF(end of file)
    );

    mime = getMimeType(fullPath);
    log("peekFile function's mime value is " + mime);
    return true;
}