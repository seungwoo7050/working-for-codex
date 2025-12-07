/**
 * Snapshot 002: poll() Event Loop
 * 
 * TCP 소켓 서버 + poll() 기반 이벤트 멀티플렉싱
 * - 여러 클라이언트 연결 동시 관리 가능
 * - 논블로킹 I/O
 */

#include "network/Socket.hpp"
#include "network/Poller.hpp"
#include <iostream>
#include <cstdlib>
#include <csignal>
#include <unistd.h>
#include <fcntl.h>
#include <map>
#include <cerrno>

static volatile bool g_running = true;

void signalHandler(int signal) {
    if (signal == SIGINT || signal == SIGTERM) {
        std::cout << "\nShutting down server..." << std::endl;
        g_running = false;
    }
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <port> <password>" << std::endl;
        return 1;
    }

    int port = std::atoi(argv[1]);
    if (port <= 0 || port > 65535) {
        std::cerr << "Error: Invalid port number" << std::endl;
        return 1;
    }

    std::string password = argv[2];
    if (password.empty()) {
        std::cerr << "Error: Password cannot be empty" << std::endl;
        return 1;
    }

    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);
    signal(SIGPIPE, SIG_IGN);

    Socket serverSocket;
    Poller poller;
    std::map<int, std::string> clientBuffers;

    if (!serverSocket.create() || !serverSocket.setReuseAddr() ||
        !serverSocket.setNonBlocking() || !serverSocket.bind(port) ||
        !serverSocket.listen()) {
        std::cerr << "Error: Failed to initialize server" << std::endl;
        return 1;
    }

    poller.addFd(serverSocket.getFd(), POLLIN);
    std::cout << "Server listening on port " << port << std::endl;

    while (g_running) {
        int ret = poller.poll(1000);

        if (ret < 0) {
            if (errno == EINTR) continue;
            break;
        }
        if (ret == 0) continue;

        auto fds = poller.getFds();
        for (int fd : fds) {
            if (poller.hasEvent(fd, POLLERR) || poller.hasEvent(fd, POLLHUP)) {
                if (fd != serverSocket.getFd()) {
                    std::cout << "Client disconnected: fd=" << fd << std::endl;
                    poller.removeFd(fd);
                    clientBuffers.erase(fd);
                    close(fd);
                }
                continue;
            }

            if (poller.hasEvent(fd, POLLIN)) {
                if (fd == serverSocket.getFd()) {
                    // Accept new client
                    int clientFd = serverSocket.accept();
                    if (clientFd >= 0) {
                        int flags = fcntl(clientFd, F_GETFL, 0);
                        fcntl(clientFd, F_SETFL, flags | O_NONBLOCK);
                        poller.addFd(clientFd, POLLIN);
                        clientBuffers[clientFd] = "";
                        std::cout << "New client connected: fd=" << clientFd << std::endl;
                    }
                } else {
                    // Read data from client
                    char buffer[512];
                    ssize_t bytes = recv(fd, buffer, sizeof(buffer) - 1, 0);
                    if (bytes <= 0) {
                        if (bytes == 0 || (errno != EAGAIN && errno != EWOULDBLOCK)) {
                            std::cout << "Client disconnected: fd=" << fd << std::endl;
                            poller.removeFd(fd);
                            clientBuffers.erase(fd);
                            close(fd);
                        }
                    } else {
                        buffer[bytes] = '\0';
                        clientBuffers[fd] += buffer;
                        std::cout << "Received from fd=" << fd << ": " << buffer;
                        // Echo back (for testing)
                        send(fd, buffer, bytes, MSG_NOSIGNAL);
                    }
                }
            }
        }
    }

    // Cleanup
    for (auto& [fd, buf] : clientBuffers) {
        close(fd);
    }

    std::cout << "Server stopped." << std::endl;
    return 0;
}
