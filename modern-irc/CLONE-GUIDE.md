# 📓 modern-irc (modern-irc/) 클론코딩 가이드

> RFC 1459/2810-2813 기반 모던 IRC 서버 - TCP 소켓 + poll() 이벤트 멀티플렉싱

## 프로젝트 개요

| 항목 | 내용 |
|------|------|
| **기술 스택** | C++17, TCP Socket, poll(), INI Config, File Logging |
| **핵심 기능** | IRC 프로토콜, 채널/메시지 시스템, 오퍼레이터 명령, Rate Limiting |
| **예상 기간** | 4-6주 |
| **난이도** | ⭐⭐⭐ |

---

## v1.0.0: Core Functionality (2주)

> 📅 **권장 기간**: 2025년 3월 1일 ~ 3월 15일

### 📖 사전 학습
```
📚 읽어야 할 Prerequisite
├── prerequisite/c++/01-cpp17-server-ffmpeg-minimum-basics.md
│   └── C++ 기본 문법, 컴파일러, 빌드 ⭐
├── prerequisite/server-basic/02-io-model-blocking-nonblocking.md
│   └── 블로킹/논블로킹, I/O 멀티플렉싱 ⭐⭐
└── prerequisite/server-basic/03-tcp-socket-kernel-buffer.md
    └── TCP 소켓, 커널 버퍼, 수신/송신 ⭐⭐
```

### 📋 작업 내용
| 순서 | 작업 | Design 문서 | 핵심 파일 |
|------|------|-------------|----------|
| 1.0.1 | TCP 소켓 서버 | `design/v1.0.0-core-functionality.md` | `network/Socket.cpp` |
| 1.0.2 | poll() 이벤트 루프 | 〃 | `network/Poller.cpp` |
| 1.0.3 | 클라이언트 연결 관리 | 〃 | `core/Client.cpp` |
| 1.0.4 | IRC 메시지 파싱 | 〃 | `core/Message.cpp` |
| 1.0.5 | 인증 (PASS/NICK/USER) | 〃 | `commands/AuthCommands.cpp` |
| 1.0.6 | 채널 시스템 | 〃 | `core/Channel.cpp`, `commands/ChannelCommands.cpp` |
| 1.0.7 | 메시지 전송 (PRIVMSG) | 〃 | `commands/MessageCommands.cpp` |

### 🔖 커밋 포인트
```bash
# [📅 2025-03-01 20:00:00]
GIT_AUTHOR_DATE="2025-03-01 20:00:00" GIT_COMMITTER_DATE="2025-03-01 20:00:00" \
git commit -m "feat(network): implement TCP socket server with poll()"

# [📅 2025-03-03 21:30:00]
GIT_AUTHOR_DATE="2025-03-03 21:30:00" GIT_COMMITTER_DATE="2025-03-03 21:30:00" \
git commit -m "feat(core): add Client and Channel classes"

# [📅 2025-03-05 19:45:00]
GIT_AUTHOR_DATE="2025-03-05 19:45:00" GIT_COMMITTER_DATE="2025-03-05 19:45:00" \
git commit -m "feat(core): implement IRC message parser"

# [📅 2025-03-08 15:30:00]
GIT_AUTHOR_DATE="2025-03-08 15:30:00" GIT_COMMITTER_DATE="2025-03-08 15:30:00" \
git commit -m "feat(auth): implement PASS, NICK, USER commands"

# [📅 2025-03-10 20:15:00]
GIT_AUTHOR_DATE="2025-03-10 20:15:00" GIT_COMMITTER_DATE="2025-03-10 20:15:00" \
git commit -m "feat(channel): implement JOIN, PART, NAMES, LIST"

# [📅 2025-03-13 21:00:00]
GIT_AUTHOR_DATE="2025-03-13 21:00:00" GIT_COMMITTER_DATE="2025-03-13 21:00:00" \
git commit -m "feat(message): implement PRIVMSG, NOTICE"

# [📅 2025-03-15 17:00:00]
GIT_AUTHOR_DATE="2025-03-15 17:00:00" GIT_COMMITTER_DATE="2025-03-15 17:00:00" \
git commit -m "feat(server): implement PING, PONG, QUIT"
GIT_COMMITTER_DATE="2025-03-15 17:15:00" git tag -a v1.0.0 -m "Core IRC functionality"
```

### ✅ 완료 기준
- [ ] 클라이언트 연결/인증 동작
- [ ] 채널 참가/퇴장 동작
- [ ] 메시지 전송 (채널/DM) 동작
- [ ] netcat으로 기본 테스트 통과

---

## v2.0.0: Advanced Features (1.5주)

> 📅 **권장 기간**: 2025년 3월 16일 ~ 3월 30일

### 📋 작업 내용
| 순서 | 작업 | Design 문서 | 핵심 파일 |
|------|------|-------------|----------|
| 2.0.1 | 채널 모드 (+i,+t,+k,+o,+l) | `design/v2.0.0-advanced-features.md` | `core/Channel.cpp` |
| 2.0.2 | 오퍼레이터 명령 (KICK,INVITE,TOPIC) | 〃 | `commands/OperatorCommands.cpp` |
| 2.0.3 | 정보 명령 (WHOIS,WHO) | 〃 | `commands/InfoCommands.cpp` |
| 2.0.4 | RFC 에러 코드 구현 | 〃 | 전체 |
| 2.0.5 | 토픽 영속화 | 〃 | `utils/TopicPersistence.cpp` |

### 🔖 커밋 포인트
```bash
# [📅 2025-03-17 21:00:00]
GIT_AUTHOR_DATE="2025-03-17 21:00:00" GIT_COMMITTER_DATE="2025-03-17 21:00:00" \
git commit -m "feat(channel): implement channel modes (+i, +t, +k, +o, +l)"

# [📅 2025-03-20 20:30:00]
GIT_AUTHOR_DATE="2025-03-20 20:30:00" GIT_COMMITTER_DATE="2025-03-20 20:30:00" \
git commit -m "feat(operator): implement KICK, INVITE, TOPIC commands"

# [📅 2025-03-23 16:00:00]
GIT_AUTHOR_DATE="2025-03-23 16:00:00" GIT_COMMITTER_DATE="2025-03-23 16:00:00" \
git commit -m "feat(info): implement WHOIS, WHO commands"

# [📅 2025-03-26 19:45:00]
GIT_AUTHOR_DATE="2025-03-26 19:45:00" GIT_COMMITTER_DATE="2025-03-26 19:45:00" \
git commit -m "feat(error): implement RFC error codes (21+)"

# [📅 2025-03-30 15:30:00]
GIT_AUTHOR_DATE="2025-03-30 15:30:00" GIT_COMMITTER_DATE="2025-03-30 15:30:00" \
git commit -m "feat(persist): implement topic persistence"
GIT_COMMITTER_DATE="2025-03-30 15:45:00" git tag -a v2.0.0 -m "Advanced IRC features"
```

### ✅ 완료 기준
- [ ] 채널 모드 설정/해제 동작
- [ ] 오퍼레이터 전용 명령어 동작
- [ ] RFC 에러 코드 응답 확인

---

## v3.0.0: Production Infrastructure (1.5주)

> 📅 **권장 기간**: 2025년 3월 31일 ~ 4월 15일

### 📋 작업 내용
| 순서 | 작업 | Design 문서 | 핵심 파일 |
|------|------|-------------|----------|
| 3.0.1 | 설정 파일 시스템 | `design/v3.0.0-production-infrastructure.md` | `utils/Config.cpp` |
| 3.0.2 | 로깅 시스템 | 〃 | `utils/Logger.cpp` |
| 3.0.3 | Rate Limiting | 〃 | `core/Server.cpp` |
| 3.0.4 | 관리자 명령 (OPER,KILL,DIE) | 〃 | `commands/OperatorCommands.cpp` |
| 3.0.5 | 서버 통계 (STATS,INFO) | 〃 | `commands/InfoCommands.cpp` |

### 🔖 커밋 포인트
```bash
# [📅 2025-04-02 20:00:00]
GIT_AUTHOR_DATE="2025-04-02 20:00:00" GIT_COMMITTER_DATE="2025-04-02 20:00:00" \
git commit -m "feat(config): implement INI-style configuration file system"

# [📅 2025-04-05 14:30:00]
GIT_AUTHOR_DATE="2025-04-05 14:30:00" GIT_COMMITTER_DATE="2025-04-05 14:30:00" \
git commit -m "feat(logger): implement multi-level logging to console and file"

# [📅 2025-04-08 21:15:00]
GIT_AUTHOR_DATE="2025-04-08 21:15:00" GIT_COMMITTER_DATE="2025-04-08 21:15:00" \
git commit -m "feat(rate): implement rate limiting for flood protection"

# [📅 2025-04-12 15:45:00]
GIT_AUTHOR_DATE="2025-04-12 15:45:00" GIT_COMMITTER_DATE="2025-04-12 15:45:00" \
git commit -m "feat(admin): implement OPER, KILL, REHASH, DIE commands"

# [📅 2025-04-15 17:00:00]
GIT_AUTHOR_DATE="2025-04-15 17:00:00" GIT_COMMITTER_DATE="2025-04-15 17:00:00" \
git commit -m "feat(stats): implement STATS, INFO, VERSION, TIME, LUSERS"
GIT_COMMITTER_DATE="2025-04-15 17:15:00" git tag -a v3.0.0 -m "Production-ready IRC server"
```

### ✅ 완료 기준
- [ ] 설정 파일 로드/리로드 동작
- [ ] 로그 파일 출력 확인
- [ ] Rate limiting 플러드 방지 테스트
- [ ] 관리자 명령어 동작