#include "../include/ConnectionManager.hpp"
#include "../include/Logger.hpp"
#include "../include/HttpParser.hpp"
#include "../include/FileServer.hpp"
#include <thread>   // for std::thread
#include <unistd.h> // for close()

void handleClient(int client_fd, std::string docRoot)
{
    // curl -i http://localhost:8080/foo
    HttpRequest req = receiveRequest(client_fd);
    log("Method: " + req.method);
    log("Path: " + req.path);
    log("Version: " + req.version);
    for (const auto &[name, value] : req.headers)
    {
        log(name + ": " + value);
    }
    if (req.method == "OPTIONS")
    {
        // curl -X OPTIONS -i http://localhost:8080/index.html
        std::ostringstream resp;
        resp << "HTTP/1.1 204 No Content\r\n"
             << "Allow: GET, HEAD, OPTIONS\r\n"
             << "Connection: close \r\n\r\n";
        sendRaw(client_fd, resp.str());
        close(client_fd);
        return;
    }
    if (req.method == "HEAD")
    {
        // curl -I http://localhost:8080/index.html
        std::string dummy, mime;
        bool found = peekFile(req.path, docRoot, dummy, mime);
        log("docRoot is " + docRoot);
        if (!found)
        {
            sendResponse(client_fd, "404", "Not Found");
        }
        else
        {
            std::ostringstream hdr;
            hdr << "HTTP/1.1 200 OK\r\n"
                << "Content-Type: " << mime << "\r\n"
                << "Content-Length: " << dummy.size() << "\r\n"
                << "Connection: close \r\n\r\n";
            sendRaw(client_fd, hdr.str());
        }
        close(client_fd);
        return;
    }
    serveStaticFile(client_fd, req.path, docRoot);

    close(client_fd);
}

void handleClient(SSL *ssl)
{
    // Read HTTP request using SSL
    // curl -i https://localhost:8080/index.html --insecure

    HttpRequest req = receiveRequest(ssl);
    log("Method: " + req.method);
    log("Path: " + req.path);
    log("Version: " + req.version);

    for (const auto &[name, value] : req.headers)
    {
        log(name + ": " + value);
    }

    // Serve static file using SSL
    serveStaticFile(ssl, req.path, "../public");

    // Close SSL connection properly
    SSL_shutdown(ssl);
    SSL_free(ssl);
}

void spawnClientThread(int client_fd, std::string docRoot)
{
    // Spawn a new thread to handle the client connection
    // - This line creates a new std::thread object.
    // - The thread entry function is handleClient, with client_fd as its argument.
    // - The newly created thread immediately begins executing handleClient(client_fd) on its own stack.
    std::thread t([client_fd, docRoot]()
                  { handleClient(client_fd, docRoot); });

    // Detach the thread from the main thread
    // - Calling detach() makes this new thread independent of the main thread.
    // - The main thread does not wait for it to finish and immediately continues to the next accept().
    // - The detached thread cleans up its own resources once its work (reading request, serving file, closing socket) is done.
    t.detach();
}

void spawnClientThread(int client_fd, SSL_CTX *sslCtx)
{
    std::thread t([client_fd, sslCtx]()
                  {
        SSL *ssl = SSL_new(sslCtx);
        SSL_set_fd(ssl, client_fd);
        
        // Perform SSL/TLS handshake
        if (SSL_accept(ssl) <= 0)
        {
            ERR_print_errors_fp(stderr);
            SSL_free(ssl);
            close(client_fd);
            return;
        }
        else
        {
            handleClient(ssl);
        }
        SSL_shutdown(ssl);
        SSL_free(ssl);
        close(client_fd); });

    t.detach();
}