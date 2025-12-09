> ⚠️ 이 리포지토리는 **학습용 토이 프로젝트**입니다.
>   실서비스 운영/보안/장애 대응을 전제로 설계되지 않았습니다.

# multi-chat-server (CPP-C0.3)
여러 클라이언트가 동시에 접속하는 간단한 채팅 서버 예제입니다.

## Version Roadmap
- CPP-C0.3: 멀티 클라이언트 처리, 브로드캐스트 로직

## Implementation Order (Files)
1. `Server.h`, `Server.cpp`
2. `ClientHandler.h`, `ClientHandler.cpp`
3. `main.cpp`

## Troubleshooting
- `bind`/`listen` 실패: 포트 충돌 여부 확인 (`ss -lntp | grep <포트>`)
- 클라이언트 메시지 손실: 쓰레드 동기화 객체(뮤텍스) 사용 여부 확인
- 소켓 닫힘 오류: 클라이언트 종료 시 소켓을 올바르게 shutdown/close
