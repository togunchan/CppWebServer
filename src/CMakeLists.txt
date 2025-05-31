# 1. Compile the SocketManager module as a library
add_library(SocketManager STATIC
    SocketManager.cpp
)

# 2. Compile the HttpParser module as a library
add_library(HttpParser STATIC
    HttpParser.cpp
)

# 3. Compile the FileServer module as a library
add_library(FileServer STATIC
    FileServer.cpp
)

# 4. Compile the Logger module as a library
add_library(Logger STATIC
    Logger.cpp
)

# 5. Compile the ConnectionManager module (handleClient + spawnClientThread)
add_library(ConnectionManager STATIC
    ConnectionManager.cpp
)

# 6. Compile the Config module as a library
add_library(Config STATIC
    Config.cpp
)

# 7. Compile the SSLManager module as a library
add_library(SSLManager STATIC
    SSLManager.cpp
)

add_library(HttpResponse STATIC
    HttpResponse.cpp
)

target_link_libraries(SSLManager
    PUBLIC
        OpenSSL::SSL
        OpenSSL::Crypto
)

# 8. Create the main executable
add_executable(CppWebServer
    main.cpp
)

# 8. Link the executable with the libraries
target_link_libraries(CppWebServer
    PRIVATE
        SocketManager
        HttpParser
        FileServer
        Logger
        ConnectionManager
        Config
        SSLManager
        HttpResponse
        pthread      # Required for std::thread
)
