#include "../include/server.hpp"
#include <sstream>       // for std::istringstream
#include <unordered_map> // for std::unordered_map
#include <fstream>       // for std::ifstream

int createTcpSocket()
{
    // create TCP socket
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0)
    {
        perror("socket");
        return 1;
    }
    return server_fd;
}

void bindSocket(int fd, uint16_t port)
{
    // bind to port 8080 on any interface
    sockaddr_in addr{}; // Declare and zero-initialize the IPv4 address structure
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = INADDR_ANY;
    if (bind(fd, reinterpret_cast<sockaddr *>(&addr), sizeof(addr)) < 0)
    {
        perror("bind");
        exit(EXIT_FAILURE);
    }
}

void startListening(int fd)
{
    if (listen(fd, BACKLOG) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }
    std::cout << "Server listening on port" << PORT << std::endl;
}

void echoLoop(int client_fd)
{
    std::cout << "I am in echoLoop function" << std::endl;
    constexpr size_t BUFFER_SIZE = 1024;
    char buffer[BUFFER_SIZE];
    while (true)
    {
        // Zero out the buffer before reading
        std::memset(buffer, 0, BUFFER_SIZE);
        ssize_t bytes_read = read(client_fd, buffer, BUFFER_SIZE - 1);
        if (bytes_read < 0)
        {
            perror("read");
            break;
        }
        else if (bytes_read == 0)
        {
            std::cout << "Client fd= " << client_fd << "disconnected\n";
            break;
        }

        // Send back exactly what we receive
        size_t total_sent = 0;
        while (total_sent < bytes_read)
        {
            ssize_t sent = write(client_fd, buffer + total_sent, bytes_read - total_sent);
            if (sent < 0)
            {
                perror("write");
                break;
            }
            total_sent += sent;
        }
    }
}

int waitForClient(int server_fd)
{
    int client_fd = accept(server_fd, nullptr, nullptr);
    if (client_fd >= 0)
    {
        // const char *msg = "Hello World\n";
        // send(client_fd, msg, strlen(msg), 0);
        //  close(client_fd);
        std::cout << "New client connected: fd= " << client_fd << "\n";
    }
    else
    {
        perror("accept");
        exit(EXIT_FAILURE);
    }

    // close(client_fd);
    return client_fd;
}

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
    if (path == "/")
        fullPath += "index.html";

    // input file stream
    // In text mode, some systems translate end-of-line sequences ("\r\n" <-> "\n");
    // in binary mode, no such translations occur and bytes are read exactly as-is.
    std::ifstream file(fullPath, std::ios::binary);
    if (!file)
    {
        sendResponse(fd, "404 Not Found", "text/plain");
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

int main()
{
    int server_fd = createTcpSocket();
    bindSocket(server_fd, PORT);
    startListening(server_fd);
    int client_fd = waitForClient(server_fd);

    // curl -i http://localhost:8080/foo
    HttpRequest req = receiveRequest(client_fd);
    std::cout << "Method: " << req.method << std::endl;
    std::cout << "Path: " << req.path << std::endl;
    std::cout << "Version: " << req.version << std::endl;

    serveStaticFile(client_fd, req.path, "./public/");

    // Headers
    for (const auto &[name, value] : req.headers)
    {
        std::cout << name << ": " << value << std::endl;
    }

    close(client_fd);
    close(server_fd);

    return 0;
}