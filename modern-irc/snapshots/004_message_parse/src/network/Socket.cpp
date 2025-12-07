#include "Socket.hpp"
#include <iostream>
#include <cstring>
#include <cerrno>

Socket::Socket() : fd(-1), port(0) {
    memset(&address, 0, sizeof(address));
}

Socket::Socket(int existingFd) : fd(existingFd), port(0) {
    memset(&address, 0, sizeof(address));
}

Socket::~Socket() {
    if (fd >= 0) {
        close();
    }
}

bool Socket::create() {
    fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) {
        std::cerr << "Error creating socket: " << strerror(errno) << std::endl;
        return false;
    }
    return true;
}

bool Socket::bind(int p) {
    port = p;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    if (::bind(fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        std::cerr << "Error binding socket to port " << port << ": " << strerror(errno) << std::endl;
        return false;
    }
    return true;
}

bool Socket::listen(int backlog) {
    if (::listen(fd, backlog) < 0) {
        std::cerr << "Error listening on socket: " << strerror(errno) << std::endl;
        return false;
    }
    return true;
}

int Socket::accept() {
    struct sockaddr_in clientAddr;
    socklen_t clientAddrLen = sizeof(clientAddr);

    int clientFd = ::accept(fd, (struct sockaddr*)&clientAddr, &clientAddrLen);
    if (clientFd < 0) {
        if (errno != EAGAIN && errno != EWOULDBLOCK) {
            std::cerr << "Error accepting connection: " << strerror(errno) << std::endl;
        }
        return -1;
    }
    return clientFd;
}

bool Socket::setNonBlocking() {
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags < 0) return false;
    if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) < 0) return false;
    return true;
}

bool Socket::setReuseAddr() {
    int opt = 1;
    if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) return false;
    return true;
}

int Socket::getFd() const { return fd; }
int Socket::getPort() const { return port; }

void Socket::close() {
    if (fd >= 0) {
        ::close(fd);
        fd = -1;
    }
}
