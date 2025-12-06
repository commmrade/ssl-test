#pragma once

#include <unistd.h>
#include <openssl/ssl.h>


struct Socket {
    int sock_;
    Socket(int sock)  : sock_(sock) {}
    ~Socket() {
        close(sock_);
    }
};

struct SslCtx {
    SSL_CTX* ctx_;
    SslCtx(SSL_CTX* ctx) : ctx_(ctx) {}
    ~SslCtx() {
        SSL_CTX_free(ctx_);
    }
};
struct Ssl {
    SSL* ssl_;
    Ssl(SSL* ssl) : ssl_(ssl) {}
    ~Ssl() {
        SSL_shutdown(ssl_);
        SSL_shutdown(ssl_);
        SSL_free(ssl_);
    }
};