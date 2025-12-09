#pragma once
#include <mutex>
#include <set>
#include <string>

// [FILE]
// - 목적: 멀티 채팅 서버 인터페이스 정의.
// - 주요 역할: 클라이언트 소켓 집합 관리, 브로드캐스트 기능 제공.
// - 관련 토이 버전: [CPP-C0.3]
// - 권장 읽는 순서: 필드 -> 메서드 -> main.cpp 사용.
//
// [LEARN] 단순 멀티스레드 서버를 설계할 때 필요한 동기화 객체를 학습한다.
class Server {
public:
    explicit Server(int port);
    void run();
private:
    int port_;
    int serverFd_;
    std::set<int> clients_;
    std::mutex mutex_;

    void broadcast(const std::string& message, int senderFd);
};
