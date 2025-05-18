#include "../include/ConnectionManager.hpp"
#include "../include/Logger.hpp"
#include "../include/HttpParser.hpp"
#include "../include/FileServer.hpp"
#include <thread>   // for std::thread
#include <unistd.h> // for close()

void handleClient(int client_fd)
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
    serveStaticFile(client_fd, req.path, "./public");

    close(client_fd);
}

void spawnClientThread(int client_fd)
{
    // Spawn a new thread to handle the client connection
    // - This line creates a new std::thread object.
    // - The thread entry function is handleClient, with client_fd as its argument.
    // - The newly created thread immediately begins executing handleClient(client_fd) on its own stack.
    std::thread t(handleClient, client_fd);

    // Detach the thread from the main thread
    // - Calling detach() makes this new thread independent of the main thread.
    // - The main thread does not wait for it to finish and immediately continues to the next accept().
    // - The detached thread cleans up its own resources once its work (reading request, serving file, closing socket) is done.
    t.detach();
}