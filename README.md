# CppWebServer

**A lightweight, modular, and extensible C++ HTTP/HTTPS server.**  
CppWebServer is built for serving static files securely and efficiently with support for essential HTTP methods, SSL/TLS encryption, multithreading, and content negotiation. Designed with modern C++17 practices and a modular architecture using CMake.

---

## Table of Contents

1. [Features](#features)  
2. [Prerequisites](#prerequisites)  
3. [Project Structure](#project-structure)  
4. [Building from Source](#building-from-source)  
5. [Configuration](#configuration)  
6. [Running the Server](#running-the-server)  
7. [Usage Examples](#usage-examples)  
8. [Error Handling](#error-handling)  
9. [Content Negotiation](#content-negotiation)  
10. [SSL/TLS (HTTPS) Support](#ssltls-https-support)  
11. [Logging](#logging)  
12. [Extending & Contributing](#extending--contributing)  
13. [Roadmap & Future Work](#roadmap--future-work)  
14. [License](#license)

---

## Features

- **Sockets:** Core TCP/IP support via BSD sockets (IPv4), with HTTP and HTTPS listeners.
- **HTTP Support:** GET, HEAD, OPTIONS methods; dynamic status and headers.
- **Static Files:** Serve from a customizable `docRoot` with MIME detection and index.html fallback.
- **Concurrency:** `std::thread`-based handling for each connection with thread-safe logging.
- **Modularity:** Individual components for sockets, HTTP, logging, config, SSL, etc.
- **Custom JSON Config:** Lightweight JSON parser for loading runtime parameters.
- **HTTPS Support:** Built-in SSL/TLS with OpenSSL.
- **Error Responses:** 400, 404, 405, 406, 500 with HTML messages.
- **Content Negotiation:** Honors `Accept` header for MIME type filtering.
- **CMake Build System:** Modern modular `CMakeLists.txt`.

---

## Prerequisites

- **Compiler:** C++17 (GCC ≥ 7, Clang ≥ 8)
- **CMake:** ≥ 3.10
- **OpenSSL:** Required for HTTPS

### macOS

```sh
brew install cmake openssl pkg-config
```

### Ubuntu/Debian

```sh
sudo apt update
sudo apt install -y build-essential cmake libssl-dev
```

---

## Project Structure

```
CppWebServer/
├── CMakeLists.txt
├── config.json
├── include/
│   ├── *.hpp                 # Component headers
├── src/
│   ├── *.cpp                 # Implementations + main.cpp
├── public/
│   ├── index.html
│   └── style.css
```

---

## Building from Source

```sh
git clone https://github.com/togunchan/CppWebServer.git
cd CppWebServer
mkdir build && cd build
cmake ..
make
```

**Note (macOS OpenSSL):**

```sh
cmake -DCMAKE_BUILD_TYPE=Release \
  -DOPENSSL_ROOT_DIR=/usr/local/opt/openssl \
  -DOPENSSL_INCLUDE_DIR=/usr/local/opt/openssl/include \
  -DOPENSSL_LIBRARIES=/usr/local/opt/openssl/lib ..
```

---

## Configuration

**Sample `config.json`:**

```json
{
  "port": 8080,
  "sslPort": 8443,
  "docRoot": "./public",
  "maxThreads": 4
}
```

Values are parsed by a custom lightweight JSON parser. Missing or malformed files trigger detailed errors.

---

## Running the Server

1. Ensure `server.crt` and `server.key` are in place.
2. Launch the server:

```sh
./build/src/CppWebServer
```

**Sample Output:**

```
port: 8080
sslPort: 8443
docRoot: ./public
maxThreads: 4
Server listening on port 8080
Server listening on port 8443
```

---

## Usage Examples

```sh
curl -i http://localhost:8080/index.html          # 200 OK
curl -I http://localhost:8080/index.html          # HEAD
curl -X OPTIONS -i http://localhost:8080/index.html  # OPTIONS
curl -i http://localhost:8080/missing.html        # 404 Not Found
curl -X POST -i http://localhost:8080/index.html  # 405 Method Not Allowed
curl -H "Accept: image/png" -i http://localhost:8080/index.html  # 406 Not Acceptable
```

**HTTPS Example:**

```sh
curl -k https://localhost:8443/index.html
```

---

## Error Handling

| Code | Description                        |
|------|------------------------------------|
| 400  | Malformed HTTP request             |
| 404  | File not found                     |
| 405  | Unsupported HTTP method            |
| 406  | MIME type not acceptable           |
| 500  | Internal server error              |

All errors return minimal HTML pages via `sendErrorResponse()`.

---

## Content Negotiation

MIME matching from `Accept` header supports:

- `*/*` → Accept all
- `type/*` → Accept all subtypes (e.g. `text/*`)
- `type/subtype` → Exact match

Fallback: `*/*` if `Accept` is missing.

---

## SSL/TLS (HTTPS) Support

- Uses OpenSSL.
- Separate listener on `sslPort`.
- SSL handshake with `SSL_accept()`.
- Handles encrypted reads/writes via `SSL_read()` / `SSL_write()`.

**To generate a self-signed certificate:**

```sh
openssl req -x509 -newkey rsa:4096 -keyout server.key -out server.crt \
  -days 365 -nodes -subj "/CN=localhost"
```

---

## Logging

- Thread-safe logging via `std::mutex`.
- Output to `std::cout`.
- Future: log levels, file output, rotation.

```cpp
void log(const std::string& msg) {
    std::lock_guard<std::mutex> lock(logMutex);
    std::cout << msg << std::endl;
}
```

---

## Extending & Contributing

1. **Clone & Build**

```sh
git clone https://github.com/togunchan/CppWebServer.git
cd CppWebServer
mkdir build && cd build
cmake ..
make
```

2. **Feature Branch**

```sh
git checkout -b feature/my-feature
```

3. **Coding Guidelines**

- 4-space indentation
- Braces on new lines
- Doxygen-style comments

4. **Submit a PR**

Push and open a Pull Request describing your changes.

### Testing (Planned)

- **Unit Testing:** `HttpParser`, `Config`, `getMimeType`, `isAcceptable`
- **Integration:** Full server runs using `curl`, `openssl`, etc.
- **CI:** GitHub Actions or similar for automatic builds.

---

## Roadmap & Future Work

- [ ] Thread Pool
- [ ] HTTP Keep-Alive
- [ ] LRU Caching
- [ ] Gzip Compression
- [ ] Routing & Dynamic Content
- [ ] WebSocket Support
- [ ] HTTP/2 & HTTP/3
- [ ] Security Enhancements
- [ ] Let's Encrypt Integration
- [ ] Structured Logging & Metrics
- [ ] Complete Test Suite

---

## License

This project is licensed under the [MIT License](LICENSE).  
Feel free to use, modify, and distribute.

---

Developed and maintained by [Murat Togunçhan Düzgün](https://github.com/togunchan)
