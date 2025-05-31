#include "../include/ConnectionManager.hpp"
#include "../include/Logger.hpp"
#include "../include/HttpParser.hpp"
#include "../include/FileServer.hpp"
#include <thread>   // for std::thread
#include <unistd.h> // for close()
#include "../include/HttpResponse.hpp"
#include "../include/ContentNegotiation.hpp"

void handleClient(int client_fd, const std::string docRoot)
{
    try
    {
        // curl -i http://localhost:8080/foo
        HttpRequest req;
        try
        {
            req = receiveRequest(client_fd);
            log("Method: " + req.method);
            log("Path: " + req.path);
            log("Version: " + req.version);
            for (const auto &[name, value] : req.headers)
            {
                log(name + ": " + value);
            }
        }
        catch (const std::exception &e)
        {
            std::string body = "<html><body><h1>400 Bad Request</h1></body></html>";
            sendErrorResponse(client_fd, 400, "Bad Request", body);
            close(client_fd);
            return;
        }

        if (req.method == "OPTIONS")
        {
            log("if (req.method == OPTIONS ) worked");
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
            log("if (req.method == HEAD ) worked");
            // curl -I http://localhost:8080/index.html
            std::string dummy, mime;
            bool found = peekFile(req.path, docRoot, dummy, mime);
            log("docRoot is " + docRoot);
            if (!found)
            {
                // curl -i http://localhost:8080/notexist.html
                std::string body = "<html><body><h1>404 Not Found</h1></body></html>";
                sendErrorResponse(client_fd, 404, "Not Found", body);
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
        if (req.method == "GET")
        {
            log("if (req.method == GET ) worked");
            std::string content, mime;
            bool found = peekFile(req.path, docRoot, content, mime);

            if (!found)
            {
                std::string body404 = "<html><body><h1>404 Not Found</h1></body></html>";
                sendErrorResponse(client_fd, 404, "Not Found", body404);
                close(client_fd);
                return;
            }

            std::string acceptHeader;
            auto it = req.headers.find("Accept");
            if (it != req.headers.end())
                acceptHeader = it->second;

            // Check if the MIME type is acceptable
            if (!isAcceptable(acceptHeader, mime))
            {
                log("if (!isAcceptable(acceptHeader, mime)) worked");
                log("acceptHeader is " + acceptHeader);
                log("mime is " + mime);
                std::string body406 = "<html><body><h1>406 Not Acceptable</h1></body></html>";
                sendErrorResponse(client_fd, 406, "Not Acceptable", body406);
                close(client_fd);
                return;
            }
            log("Entering the serveStaticFile function.");
            serveStaticFile(client_fd, req.path, docRoot);
        }

        if (req.method != "GET" && req.method != "HEAD" && req.method != "OPTIONS")
        {
            // curl -X POST -i http://localhost:8080/index.html
            std::string body = "<html><body><h1>405 Method Not Allowed</h1></body></html>";
            sendErrorResponse(client_fd, 405, "Method Not Allowed", body);
        }

        close(client_fd);
        return;
    }
    catch (const std::exception &e)
    {
        std::string body = "<html><body><h1>500 Internal Server Error</h1></body></html>";
        sendErrorResponse(client_fd, 500, "Internal Server Error", body);
        log(std::string("Internal error: ") + e.what());

        close(client_fd);
        return;
    }
}

void handleClientSSL(SSL *ssl, const std::string docRoot)
{
    // Read HTTP request using SSL
    // curl -i https://localhost:8080/index.html --insecure

    HttpRequest req = receiveRequestSSL(ssl);
    log("Method: " + req.method);
    log("Path: " + req.path);
    log("Version: " + req.version);

    for (const auto &[name, value] : req.headers)
    {
        log(name + ": " + value);
    }

    // Serve static file using SSL
    serveStaticFileSSL(ssl, req.path, "../public");

    // // Close SSL connection properly
    // SSL_shutdown(ssl);
    // SSL_free(ssl);
}

void spawnClientThreadHTTP(int client_fd, const std::string &docRoot)
{
    log("I am in the spawnClientThreadHTTP for http");
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

void spawnClientThreadHTTPS(int client_fd, SSL_CTX *sslCtx, const std::string &docRoot)
{
    log("I am in the spawnClientThreadHTTPS for https");
    std::thread t([client_fd, sslCtx, docRoot]()
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
            handleClientSSL(ssl, docRoot);
        }
        SSL_shutdown(ssl);
        SSL_free(ssl);
        close(client_fd); });

    t.detach();
}
