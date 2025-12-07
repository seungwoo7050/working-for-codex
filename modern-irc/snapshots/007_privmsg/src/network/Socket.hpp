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

    bool create();
    bool bind(int port);
    bool listen(int backlog = 128);
    int accept();
    bool setNonBlocking();
    bool setReuseAddr();
    int getFd() const;
    int getPort() const;
    void close();
};

#endif
