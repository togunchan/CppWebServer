# 1. CMake version requirement
cmake_minimum_required(VERSION 3.10)

# 2. Project name and programming language (without a specific version)
project(CppWebServer LANGUAGES CXX)

# 3. Set the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)


set(OPENSSL_ROOT_DIR   "/opt/homebrew/opt/openssl@3" CACHE PATH "Homebrew OpenSSL root")
find_package(OpenSSL REQUIRED COMPONENTS SSL Crypto)
message(STATUS "OpenSSL Include Dir: ${OPENSSL_INCLUDE_DIR}")

# 4. Add the include directory so header files can be found
include_directories(${CMAKE_SOURCE_DIR}/include)

# 5. Execute the CMakeLists.txt file inside the src subdirectory
add_subdirectory(src)

# 6. Locate OpenSSL (headers and libraries)
set(OPENSSL_ROOT_DIR   "/opt/homebrew/opt/openssl@3"    CACHE PATH "Homebrew OpenSSL root")
set(OPENSSL_INCLUDE_DIR "${OPENSSL_ROOT_DIR}/include"  CACHE PATH "")
set(OPENSSL_SSL_LIBRARY  "${OPENSSL_ROOT_DIR}/lib/libssl.dylib"   CACHE FILEPATH "")
set(OPENSSL_CRYPTO_LIBRARY "${OPENSSL_ROOT_DIR}/lib/libcrypto.dylib" CACHE FILEPATH "")
find_package(OpenSSL REQUIRED)

# 7. Make OpenSSL headers available
include_directories(${OPENSSL_INCLUDE_DIR})
message(STATUS "OpenSSL Include Dir: ${OPENSSL_INCLUDE_DIR}")

target_include_directories(CppWebServer PRIVATE ${OPENSSL_INCLUDE_DIR})
target_include_directories(HttpParser PRIVATE ${OPENSSL_INCLUDE_DIR})
target_include_directories(ConnectionManager PRIVATE ${OPENSSL_INCLUDE_DIR})
target_include_directories(SSLManager PRIVATE ${OPENSSL_INCLUDE_DIR})
target_include_directories(FileServer PRIVATE ${OPENSSL_INCLUDE_DIR})