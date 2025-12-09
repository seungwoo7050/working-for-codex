#include <arpa/inet.h>
#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

// [FILE]
// - 목적: TCP 에코 서버에 메시지를 보내고 응답을 출력하는 클라이언트 구현.
// - 주요 역할: 서버 연결, 문자열 송수신.
// - 관련 토이 버전: [CPP-C0.2]
// - 권장 읽는 순서: connect -> send/recv -> 종료 처리.
//
// [LEARN] 간단한 소켓 클라이언트 흐름을 연습한다.
int main() {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        std::cerr << "소켓 생성 실패" << std::endl;
        return 1;
    }

    sockaddr_in server{};
    server.sin_family = AF_INET;
    server.sin_port = htons(9000);
    server.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (connect(sock, reinterpret_cast<sockaddr*>(&server), sizeof(server)) < 0) {
        std::cerr << "연결 실패" << std::endl;
        return 1;
    }

    const char* message = "hello";
    send(sock, message, std::strlen(message), 0);

    char buffer[1024];
    ssize_t received = recv(sock, buffer, sizeof(buffer) - 1, 0);
    if (received > 0) {
        buffer[received] = '\0';
        std::cout << "서버 응답: " << buffer << std::endl;
    }

    close(sock);
    return 0;
}
