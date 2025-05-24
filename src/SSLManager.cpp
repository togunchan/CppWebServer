#include "SSLManager.hpp"
#include <stdexcept>
#include "../include/Exception.hpp"

SSL_CTX *createServerSSLContext(const std::string &certFile, const std::string &keyFile)
{
    SSL_library_init();
    OpenSSL_add_all_algorithms();
    SSL_load_error_strings();
    const SSL_METHOD *method = TLS_server_method();
    SSL_CTX *ctx = SSL_CTX_new(method);
    if (!ctx)
    {
        throw SSLException("Unable to create SSL context");
    }

    // load certificate and private key
    if (SSL_CTX_use_certificate_file(ctx, certFile.c_str(), SSL_FILETYPE_PEM) <= 0 ||
        SSL_CTX_use_PrivateKey_file(ctx, keyFile.c_str(), SSL_FILETYPE_PEM) <= 0)
    {
        SSL_CTX_free(ctx);
        throw std::runtime_error("Failed to load cert or key");
    }

    return ctx;
}