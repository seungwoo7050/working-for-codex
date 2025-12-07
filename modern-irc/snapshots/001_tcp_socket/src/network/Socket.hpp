#ifndef SOCKET_HPP
#define SOCKET_HPP

#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>

class Socket {
private:
    int fd;
    int port;
    struct sockaddr_in address;

public:
    Socket();
    Socket(int existingFd);
    ~Socket();

    // Server socket operations
    bool create();
    bool bind(int port);
    bool listen(int backlog = 128);
    int accept();

    // Set socket to non-blocking mode
    bool setNonBlocking();

    // Set socket options
    bool setReuseAddr();

    // Getters
    int getFd() const;
    int getPort() const;

    // Close socket
    void close();
};

#endif
