# modern-irc - RFC 1459 IRC 서버 구현

> C++17 기반 IRC (Internet Relay Chat) 서버 구현 프로젝트

---

## 목차

1. [프로젝트 개요](#1-프로젝트-개요)
2. [기술 스택](#2-기술-스택)
3. [프로젝트 구조](#3-프로젝트-구조)
4. [빌드 및 실행](#4-빌드-및-실행)
5. [구현 단계](#5-구현-단계)
6. [명령어 레퍼런스](#6-명령어-레퍼런스)
7. [설정](#7-설정)
8. [테스트](#8-테스트)
9. [학습 포인트](#9-학습-포인트)
10. [포트폴리오 가치](#10-포트폴리오-가치)

---

## 1. 프로젝트 개요

### 1-1. 소개

RFC 1459, 2810-2813을 기반으로 하는 IRC 서버 구현 프로젝트입니다. poll() 기반 I/O 멀티플렉싱으로 다중 클라이언트를 처리하며, 프로덕션 수준의 기능(설정 파일, 로깅, Rate Limiting)을 포함합니다.

### 1-2. 목표

- **Phase 1**: 기본 IRC 서버 (연결, 채널, 메시지)
- **Phase 2**: 고급 기능 (운영자 명령, 모드)
- **Phase 3**: 확장 기능 (로깅, 설정, 보안)

### 1-3. 이 프로젝트가 중요한 이유

대부분의 개발자는 HTTP/REST만 경험합니다. 이 프로젝트를 통해:
- 텍스트 기반 네트워크 프로토콜 직접 파싱 및 구현
- I/O 멀티플렉싱 (poll/select) 실습
- 상태 관리 (유저, 채널, 메시지 라우팅)
- Modern C++ 스타일의 클린 코드 작성

**면접 어드밴티지**: 소켓 프로그래밍, 프로토콜 설계, 이벤트 루프 설명 가능

---

## 2. 기술 스택

| 항목 | 사용 기술 |
|------|----------|
| **언어** | C++17 |
| **네트워킹** | BSD Sockets, poll() |
| **빌드** | Makefile |
| **테스트** | 표준 IRC 클라이언트 (irssi, WeeChat, hexchat) |
| **프로토콜** | RFC 1459 (IRC), RFC 2810-2813 |

### C++17 기능 활용

이 프로젝트는 Modern C++ 스타일로 구현되었습니다:
- 스마트 포인터 (`std::unique_ptr`, `std::shared_ptr`)로 자원 관리
- `auto`, 람다 표현식, 범위 기반 for 루프
- `std::optional`, `std::string_view` 등 유틸리티 타입
- Structured bindings, if-init 문법

---

## 3. 프로젝트 구조

```
modern-irc/
├── src/
│   ├── core/
│   │   ├── Server.cpp/hpp         # 메인 서버 클래스
│   │   ├── Client.cpp/hpp         # 클라이언트 관리
│   │   ├── Channel.cpp/hpp        # 채널 관리
│   │   └── Message.cpp/hpp        # 메시지 파싱/생성
│   ├── commands/
│   │   ├── AuthCommands.cpp       # PASS, NICK, USER
│   │   ├── ChannelCommands.cpp    # JOIN, PART, etc.
│   │   ├── MessageCommands.cpp    # PRIVMSG, NOTICE
│   │   ├── OperatorCommands.cpp   # KICK, INVITE, TOPIC, MODE
│   │   ├── InfoCommands.cpp       # WHOIS, WHO
│   │   ├── AdminCommands.cpp      # OPER, KILL, REHASH
│   │   └── ServerStatsCommands.cpp # STATS, INFO, VERSION
│   ├── network/
│   │   ├── Socket.cpp/hpp         # 소켓 래퍼
│   │   └── Poller.cpp/hpp         # poll() I/O 멀티플렉싱
│   ├── utils/
│   │   ├── Logger.cpp/hpp         # 로깅 시스템
│   │   ├── Config.cpp/hpp         # 설정 파일 파서
│   │   ├── StringUtils.cpp/hpp    # 문자열 처리
│   │   └── TopicPersistence.cpp/hpp # 토픽 영속성
│   └── main.cpp
├── config/
│   └── modern-irc.conf            # 설정 파일
├── logs/                          # 로그 디렉토리
├── tests/                         # 테스트 스크립트
├── docs/                          # 문서
├── design/                        # 설계 문서
│   ├── 0.0-initial-design.md
│   ├── v1.0.0-core-functionality.md
│   ├── v2.0.0-advanced-features.md
│   └── v3.0.0-production-infrastructure.md
└── Makefile
```

---

## 4. 빌드 및 실행

### 4-1. 빌드

```bash
make        # 빌드
make clean  # 오브젝트 파일 삭제
make fclean # 전체 삭제
make re     # 재빌드
```

### 4-2. 실행

```bash
# 기본 실행
./modern-irc <port> <password>

# 예시
./modern-irc 6667 mypassword
```

### 4-3. 클라이언트 접속

**netcat 사용:**
```bash
nc localhost 6667
PASS mypassword
NICK alice
USER alice 0 * :Alice Wonderland
JOIN #general
PRIVMSG #general :Hello, world!
```

**irssi 사용:**
```bash
irssi -c localhost -p 6667
/PASS mypassword
/NICK alice
/JOIN #general
```

**WeeChat 사용:**
```bash
weechat
/server add local localhost/6667
/set irc.server.local.password "mypassword"
/connect local
/join #general
```

---

## 5. 구현 단계

### v1.0.0: 핵심 기능 ✅

| 기능 | 설명 |
|------|------|
| TCP 서버 | poll() 기반 non-blocking I/O |
| 클라이언트 관리 | accept, read, write, close |
| 인증 | PASS, NICK, USER |
| 채널 | JOIN, PART, NAMES, LIST |
| 메시지 | PRIVMSG, NOTICE |
| 서버 명령 | PING, PONG, QUIT |

### v2.0.0: 고급 기능 ✅

| 기능 | 설명 |
|------|------|
| 채널 운영자 | KICK, INVITE, TOPIC |
| 채널 모드 | +i, +t, +k, +o, +l |
| 유저 모드 | +i, +o |
| 정보 명령 | WHOIS, WHO |
| 에러 처리 | 21+ RFC 에러 코드 |

### v3.0.0: 프로덕션 인프라 ✅

| 기능 | 설명 |
|------|------|
| 설정 파일 | INI 스타일 설정, 런타임 리로드 |
| 로깅 | 멀티 레벨 (DEBUG~CRITICAL) |
| Rate Limiting | Flood 보호 |
| 관리자 명령 | OPER, KILL, REHASH, RESTART, DIE |
| 서버 통계 | STATS, INFO, VERSION, TIME, LUSERS |

---

## 6. 명령어 레퍼런스

### 6-1. 인증

| 명령어 | 사용법 | 설명 |
|--------|--------|------|
| PASS | `PASS <password>` | 서버 비밀번호 인증 |
| NICK | `NICK <nickname>` | 닉네임 설정 |
| USER | `USER <username> <mode> <unused> :<realname>` | 사용자 정보 설정 |
| QUIT | `QUIT [:<message>]` | 서버 연결 종료 |

### 6-2. 채널 운영

| 명령어 | 사용법 | 설명 |
|--------|--------|------|
| JOIN | `JOIN <channel>` | 채널 입장 |
| PART | `PART <channel> [:<reason>]` | 채널 퇴장 |
| NAMES | `NAMES <channel>` | 채널 사용자 목록 |
| LIST | `LIST` | 전체 채널 목록 |
| TOPIC | `TOPIC <channel> [:<topic>]` | 토픽 조회/설정 |

### 6-3. 메시징

| 명령어 | 사용법 | 설명 |
|--------|--------|------|
| PRIVMSG | `PRIVMSG <target> :<message>` | 메시지 전송 |
| NOTICE | `NOTICE <target> :<message>` | 알림 전송 |

### 6-4. 운영자 명령 (Phase 2)

| 명령어 | 사용법 | 설명 |
|--------|--------|------|
| KICK | `KICK <channel> <nick> [:<reason>]` | 사용자 추방 |
| INVITE | `INVITE <nick> <channel>` | 사용자 초대 |
| MODE | `MODE <target> <modes> [<params>]` | 모드 설정 |
| WHOIS | `WHOIS <nickname>` | 사용자 정보 조회 |
| WHO | `WHO <channel>` | 채널 사용자 조회 |

### 6-5. 관리자 명령 (Phase 3) 🔐

| 명령어 | 사용법 | 설명 |
|--------|--------|------|
| OPER | `OPER <username> <password>` | 운영자 권한 획득 |
| KILL | `KILL <nickname> [:<reason>]` | 강제 연결 종료 |
| REHASH | `REHASH` | 설정 파일 리로드 |
| RESTART | `RESTART` | 서버 재시작 |
| DIE | `DIE` | 서버 종료 |

### 6-6. 서버 통계 (Phase 3) 📊

| 명령어 | 사용법 | 설명 |
|--------|--------|------|
| STATS | `STATS [u\|m\|c\|l]` | 서버 통계 |
| INFO | `INFO` | 서버 정보 |
| VERSION | `VERSION` | 서버 버전 |
| TIME | `TIME` | 서버 시간 |
| LUSERS | `LUSERS` | 사용자 통계 |

---

## 7. 설정

### 7-1. 설정 파일 위치

`config/modern-irc.conf`

### 7-2. 설정 예시

```ini
# 서버 설정
server_name = irc.local
port = 6667
max_clients = 1000

# 인증
server_password = password
operator_password = operpass

# Rate Limiting
rate_limit_enabled = true
rate_limit_messages = 10
rate_limit_window = 10

# 로깅
log_enabled = true
log_file = logs/modern-irc.log
log_level = INFO
```

### 7-3. 로그 레벨

| 레벨 | 설명 |
|------|------|
| DEBUG | 상세 디버깅 정보 |
| INFO | 일반 정보 |
| WARNING | 경고 메시지 |
| ERROR | 에러 메시지 |
| CRITICAL | 심각한 에러 |

**로그 예시:**
```
[2025-01-15 14:23:45] [INFO    ] New connection: fd=4 from 127.0.1:54321
[2025-01-15 14:23:46] [DEBUG   ] Command: alice -> NICK alice
[2025-01-15 14:23:47] [INFO    ] JOIN: alice joined #general
```

---

## 8. 테스트

### 8-1. 수동 테스트

```bash
# 터미널 1: 서버 시작
./modern-irc 6667 testpass

# 터미널 2: alice 접속
nc localhost 6667
PASS testpass
NICK alice
USER alice 0 * :Alice
JOIN #test
PRIVMSG #test :Hello from alice!

# 터미널 3: bob 접속
nc localhost 6667
PASS testpass
NICK bob
USER bob 0 * :Bob
JOIN #test
PRIVMSG #test :Hello from bob!
```

### 8-2. Rate Limiting 테스트

```bash
nc localhost 6667
PASS testpass
NICK spammer
USER spammer 0 * :Spammer
# 10초 내에 10개 이상의 메시지 전송 시 Rate Limit 발동
```

### 8-3. 관리자 명령 테스트

```bash
# 운영자 되기
OPER admin operpass

# 명령어 테스트
KILL baduser Spamming
REHASH
STATS l
```

### 8-4. Valgrind 메모리 검사

```bash
valgrind --leak-check=full --track-origins=yes ./modern-irc 6667 pass
```

---

## 9. 학습 포인트

### 9-1. 네트워크 프로그래밍

```cpp
// poll() 기반 이벤트 루프
std::vector<pollfd> fds;
while (running) {
    int ready = poll(&fds[0], fds.size(), -1);
    for (size_t i = 0; i < fds.size(); i++) {
        if (fds[i].revents & POLLIN) {
            if (i == 0) acceptNewClient();
            else handleClientData(i);
        }
    }
}
```

### 9-2. 프로토콜 파싱

```cpp
// IRC 메시지 파싱: "PRIVMSG #channel :Hello World\r\n"
Message Message::parse(const std::string& raw) {
    Message msg;
    // prefix, command, params 추출
    // CRLF 처리, 512바이트 제한
    return msg;
}
```

### 9-3. 상태 관리

```cpp
// 채널-유저 관계 관리
class Channel {
    std::string name_;
    std::string topic_;
    std::set<Client*> members_;
    std::set<Client*> operators_;
    ModeFlags modes_;
};
```

### 9-4. 습득 기술

| 영역 | 구현 |
|------|------|
| **Network I/O** | poll(), non-blocking sockets, TCP 버퍼 관리 |
| **Protocol** | RFC 표준 파싱, 상태 기계, 에러 처리 |
| **Architecture** | 이벤트 루프, 명령 패턴, 상태 관리 |
| **C++** | RAII, STL 컨테이너, Modern C++ (스마트 포인터, 람다) |

---

## 10. 포트폴리오 가치

### 10-1. 경쟁력 평가

| 항목 | 평가 |
|------|------|
| **신입 경쟁력** | ⭐⭐⭐⭐ - 네트워크 프로토콜 직접 구현은 드문 경험 |
| **이직 경쟁력** | ⭐⭐⭐ - 시스템/네트워크 기초 역량 증명 |
| **추천 대상** | 시스템 프로그래밍 입문, 네트워크 기초 학습 |

### 10-2. 어필 포인트

- RFC 표준 문서 기반 프로토콜 구현
- poll() I/O 멀티플렉싱 직접 구현
- 채널/유저 관리, 메시지 라우팅
- Modern C++17 스타일 클린 코드
- 프로덕션 수준 기능 (로깅, Rate Limiting, 설정 파일)

### 10-3. 관련 선행 지식

- `server-basic/02-io-model-blocking-nonblocking.md` - I/O 모델
- `server-basic/03-tcp-socket-kernel-buffer.md` - TCP 소켓
- `c++/01-cpp17-server-ffmpeg-minimum-basics.md` - C++ 기초

---

## 참고 자료

- [RFC 1459 - IRC Protocol](https://tools.ietf.org/html/rfc1459)
- [RFC 2810 - IRC Architecture](https://tools.ietf.org/html/rfc2810)
- [RFC 2811 - Channel Management](https://tools.ietf.org/html/rfc2811)
- [RFC 2812 - IRC Client Protocol](https://tools.ietf.org/html/rfc2812)
- [RFC 2813 - Server Protocol](https://tools.ietf.org/html/rfc2813)
- [Modern IRC Protocol](https://modern.ircdocs.horse/)
- [Beej's Guide to Network Programming](https://beej.us/guide/bgnet/)

---

## 개발 상태

- ✅ **Phase 1**: 핵심 IRC 서버 기능 (완료)
- ✅ **Phase 2**: 고급 기능 및 운영자 명령 (완료)
- ✅ **Phase 3**: 프로덕션 인프라 및 관리 (완료)

모든 Phase가 완전히 구현되고 테스트되었습니다!
