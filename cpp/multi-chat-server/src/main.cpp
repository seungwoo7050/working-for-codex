#include "Server.h"

// [FILE]
// - 목적: multi-chat-server 실행 엔트리.
// - 주요 역할: Server 인스턴스 생성 후 run 호출.
// - 관련 토이 버전: [CPP-C0.3]
// - 권장 읽는 순서: 포트 설정 -> run 호출.
//
// [LEARN] 간단한 서버 클래스를 실행하는 최소한의 엔트리 구성을 익힌다.
int main() {
    Server server(9100);
    server.run();
    return 0;
}
