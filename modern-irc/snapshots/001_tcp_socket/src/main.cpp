/**
 * Snapshot 001: TCP Socket Server
 * 
 * 이 스냅샷은 기본 TCP 소켓 서버만 구현합니다.
 * - socket() 호출로 소켓 생성
 * - bind()로 포트 바인딩
 * - listen()으로 연결 대기
 * - accept()로 클라이언트 연결 수락
 * 
 * poll() 이벤트 루프는 다음 스냅샷(002)에서 구현됩니다.
 */

#include "network/Socket.hpp"
#include <iostream>
#include <cstdlib>
#include <csignal>
#include <unistd.h>

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
        std::cerr << "Example: " << argv[0] << " 6667 mypassword" << std::endl;
        return 1;
    }

    int port = std::atoi(argv[1]);
    if (port <= 0 || port > 65535) {
        std::cerr << "Error: Invalid port number. Must be between 1 and 65535" << std::endl;
        return 1;
    }

    std::string password = argv[2];
    if (password.empty()) {
        std::cerr << "Error: Password cannot be empty" << std::endl;
        return 1;
    }

    // Set up signal handlers
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);
    signal(SIGPIPE, SIG_IGN);

    // Create socket
    Socket serverSocket;
    
    if (!serverSocket.create()) {
        std::cerr << "Error: Failed to create socket" << std::endl;
        return 1;
    }

    if (!serverSocket.setReuseAddr()) {
        std::cerr << "Error: Failed to set socket options" << std::endl;
        return 1;
    }

    if (!serverSocket.setNonBlocking()) {
        std::cerr << "Error: Failed to set non-blocking mode" << std::endl;
        return 1;
    }

    if (!serverSocket.bind(port)) {
        std::cerr << "Error: Failed to bind to port " << port << std::endl;
        return 1;
    }

    if (!serverSocket.listen()) {
        std::cerr << "Error: Failed to listen on socket" << std::endl;
        return 1;
    }

    std::cout << "Server listening on port " << port << std::endl;
    std::cout << "Password: " << password << std::endl;
    std::cout << "(Press Ctrl+C to stop)" << std::endl;

    // Simple accept loop (blocking style for this snapshot)
    // In next snapshot, we'll use poll() for event-driven I/O
    while (g_running) {
        int clientFd = serverSocket.accept();
        if (clientFd >= 0) {
            std::cout << "New client connected: fd=" << clientFd << std::endl;
            // For now, just close the connection
            // Full client handling will be in later snapshots
            close(clientFd);
        }
        // Small sleep to prevent busy-waiting
        usleep(10000);  // 10ms
    }

    std::cout << "Server stopped." << std::endl;
    return 0;
}
