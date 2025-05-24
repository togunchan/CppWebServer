#pragma once
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <string>

/*
 * Initialize OpenSSL library and load server certificate/key.
 * Creates and returns an SSL_CTX configured for server use.
 * OpenSSL objects are created on the heap, so a pointer (`SSL_CTX*`) is used.
 * This allows for connection management and manual memory deallocation.
 */
SSL_CTX *createServerSSLContext(const std::string &certFile, const std::string &keyFile);
