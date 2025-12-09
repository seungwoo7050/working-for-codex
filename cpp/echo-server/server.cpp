#include <arpa/inet.h>
#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

// [FILE]
// - 목적: 단일 클라이언트 TCP 에코 서버 구현.
// - 주요 역할: 소켓 바인딩, 수신 메시지 에코.
// - 관련 토이 버전: [CPP-C0.2]
// - 권장 읽는 순서: 소켓 생성 -> bind/listen -> accept -> read/write 루프.
//
// [LEARN] POSIX 소켓 기본 사용법을 체험한다.
int main() {
    int serverFd = socket(AF_INET, SOCK_STREAM, 0);
    if (serverFd < 0) {
        std::cerr << "소켓 생성 실패" << std::endl;
        return 1;
    }

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(9000);

    if (bind(serverFd, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) < 0) {
        std::cerr << "bind 실패" << std::endl;
        return 1;
    }
    listen(serverFd, 1);
    std::cout << "에코 서버 시작: 9000" << std::endl;

    sockaddr_in client{};
    socklen_t len = sizeof(client);
    int clientFd = accept(serverFd, reinterpret_cast<sockaddr*>(&client), &len);
    std::cout << "클라이언트 연결됨" << std::endl;

    char buffer[1024];
    while (true) {
        ssize_t received = read(clientFd, buffer, sizeof(buffer));
        if (received <= 0) break;
        write(clientFd, buffer, received);
    }

    close(clientFd);
    close(serverFd);
    return 0;
}
