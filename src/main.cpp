#include <iostream>
#include <netinet/in.h>
#include <openssl/bio.h>
#include <stdexcept>
#include <sys/socket.h>
#include <unistd.h>
#include <openssl/ssl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <array>
#include "wrappers.hpp"


int main(int, char**){
    Socket sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock.sock_ < 0) {
        throw std::runtime_error("socket() failed");
    }
    
    addrinfo *result, *p;
    addrinfo hints{};
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    int ret = getaddrinfo("www.google.com", "80", &hints, &result);
    if (ret < 0) {
        throw std::runtime_error(gai_strerror(ret));
    }

    SslCtx ssl_ctx = SSL_CTX_new(TLS_client_method());
    if (!ssl_ctx.ctx_) {
        throw std::runtime_error("Could not create SSL Context");
    }

    Ssl ssl = SSL_new(ssl_ctx.ctx_);
    if (!ssl.ssl_) {
        throw std::runtime_error("Could not create SSL object");
    }
    ret = SSL_set_fd(ssl.ssl_, sock.sock_);
    if (!ret) {
        throw std::runtime_error("Could not set FD for SSL");
    }


    for (p = result; p != nullptr; p = p->ai_next) {
        ret = connect(sock.sock_, p->ai_addr, p->ai_addrlen);
        if (ret < 0) {
            continue;
        }

        char addr[INET6_ADDRSTRLEN]{};
        const char* r = inet_ntop(p->ai_family, p->ai_addr->sa_data, addr, sizeof(addr));
        if (!r) {
            std::cerr << "Could not convert address to presentation format\n";
        }
        break;
    }
    freeaddrinfo(result);
    if (ret < 0) { 
        throw std::runtime_error("Could not connect to any addr");
    }

    ret = SSL_connect(ssl.ssl_);
    if (ret <= 0) {
        throw std::runtime_error("Could not SSL connect");
    }

    std::string get_req{
        "GET / HTTP/1.1\r\n"
        "Host: google.com\r\n"
        "Connection: close\r\n"
        "User-Agent: curl/8.0\r\n"
        "\r\n"
    };

    int wr_bytes = SSL_write(ssl.ssl_, get_req.c_str(), get_req.size());
    if (wr_bytes < 0) {
        throw std::runtime_error("Write failed");
    }
    
    std::array<char, 4096> buf{};
    int rd_bytes = SSL_read(ssl.ssl_, buf.data(), buf.size());
    if (rd_bytes < 0) {
        throw std::runtime_error("Read failed");
    } 

    std::cout << "Read " << rd_bytes << " bytes: " << buf.data() << std::endl;
    return 0;
}
