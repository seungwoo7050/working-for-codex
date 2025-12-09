#include "Server.h"
#include <arpa/inet.h>
#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <thread>
#include <unistd.h>

// [FILE]
// - 목적: 멀티 채팅 서버 동작 구현.
// - 주요 역할: 클라이언트 accept, 메시지 수신 후 브로드캐스트.
// - 관련 토이 버전: [CPP-C0.3]
// - 권장 읽는 순서: run -> 스레드 생성 -> broadcast.
//
// [LEARN] std::thread와 mutex를 활용한 간단한 동시성 제어를 익힌다.
Server::Server(int port) : port_(port), serverFd_(-1) {}

void Server::broadcast(const std::string& message, int senderFd) {
    std::lock_guard<std::mutex> lock(mutex_);
    for (int fd : clients_) {
        if (fd == senderFd) continue;
        send(fd, message.c_str(), message.size(), 0);
    }
}

void Server::run() {
    serverFd_ = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port_);
    addr.sin_addr.s_addr = INADDR_ANY;

    bind(serverFd_, reinterpret_cast<sockaddr*>(&addr), sizeof(addr));
    listen(serverFd_, 5);
    std::cout << "채팅 서버 시작: " << port_ << std::endl;

    while (true) {
        int clientFd = accept(serverFd_, nullptr, nullptr);
        {
            std::lock_guard<std::mutex> lock(mutex_);
            clients_.insert(clientFd);
        }

        std::thread([this, clientFd]() {
            char buffer[1024];
            while (true) {
                ssize_t received = recv(clientFd, buffer, sizeof(buffer) - 1, 0);
                if (received <= 0) break;
                buffer[received] = '\0';
                broadcast(buffer, clientFd);
            }
            close(clientFd);
            std::lock_guard<std::mutex> lock(mutex_);
            clients_.erase(clientFd);
        }).detach();
    }
}
