# Snapshot 005: Auth Commands

## 패치 정보
- **패치 ID**: 1.0.5
- **패치 이름**: 인증 명령어
- **대응 커밋**: `feat(commands): implement auth commands (PASS, NICK, USER, QUIT)`

## 구현 내용
- `Command` 클래스: 기본 명령어 인터페이스
- `AuthCommands`: 인증 관련 명령어 구현
  - PASS: 서버 비밀번호 인증
  - NICK: 닉네임 설정/변경
  - USER: 사용자 정보 등록
  - QUIT: 서버 연결 종료
- RFC 2812 numeric reply 상수 정의
- Server: 명령어 등록 및 처리 로직

### 포함된 파일
```
src/
├── main.cpp
├── commands/
│   ├── Command.hpp
│   └── AuthCommands.cpp/.hpp
├── core/
│   ├── Server.cpp/.hpp
│   ├── Client.cpp/.hpp
│   └── Message.cpp/.hpp
├── network/
│   ├── Socket.cpp/.hpp
│   └── Poller.cpp/.hpp
└── utils/
    └── StringUtils.cpp/.hpp
```

## 빌드 방법
```bash
make        # 빌드
./ircserv 6667 password  # 실행
make clean  # 오브젝트 파일 정리
make fclean # 전체 정리
make re     # 재빌드
```

## 테스트
```bash
nc localhost 6667
PASS password
NICK testnick
USER testuser 0 * :Test User
# 이제 등록 완료!
QUIT :bye
```

## 빌드 가능 여부
✅ 빌드 가능 (인증/등록 기능 동작)

## 다음 단계
- **Snapshot 006**: 채널 시스템 (JOIN, PART, NAMES, LIST)
