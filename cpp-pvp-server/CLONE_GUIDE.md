# C++ PVP Server — 클론코딩 가이드

> 버전별 커밋 가이드 (주니어-미드레벨 현업 개발자 기준 현실적 타임라인)

---

## 커밋 방법

아래 명령어를 그대로 복사하여 터미널에 붙여넣으면 됩니다.  
`GIT_AUTHOR_DATE`와 `GIT_COMMITTER_DATE`가 동시에 설정됩니다.

---

## 프로젝트 개요

| 항목 | 내용 |
|------|------|
| **개발 기간** | 2025-01-01 ~ 2025-05-23 (약 20주) |
| **난이도** | 중상 (C++ 시스템 프로그래밍, 네트워크, 분산 시스템) |
| **작업 페이스** | 주 5일, 일 1커밋 기준 (복잡 기능 2-4일) |

---

## 레퍼런스

### 공식 문서
- [Boost.Asio Documentation](https://www.boost.org/doc/libs/release/doc/html/boost_asio.html)
- [Boost.Beast (WebSocket)](https://www.boost.org/doc/libs/release/libs/beast/doc/html/index.html)
- [CMake Tutorial](https://cmake.org/cmake/help/latest/guide/tutorial/index.html)
- [vcpkg Getting Started](https://vcpkg.io/en/getting-started.html)
- [Protocol Buffers C++ Guide](https://protobuf.dev/getting-started/cpptutorial/)
- [gRPC C++ Quick Start](https://grpc.io/docs/languages/cpp/quickstart/)

### 넷코드/게임 서버
- [Quake 3 Network Model](https://github.com/id-Software/Quake-III-Arena) — 스냅샷/델타 원조
- [Valve Source Multiplayer Networking](https://developer.valvesoftware.com/wiki/Source_Multiplayer_Networking)
- [Gabriel Gambetta - Fast-Paced Multiplayer](https://www.gabrielgambetta.com/client-server-game-architecture.html)
- [Gaffer On Games](https://gafferongames.com/) — 게임 네트워킹 필독
- [Overwatch GDC 2017 Netcode](https://www.youtube.com/watch?v=W3aieHjyNvw)

### 라이브러리
- [redis-plus-plus](https://github.com/sewenew/redis-plus-plus) — Redis C++ 클라이언트
- [libpqxx](https://github.com/jtv/libpqxx) — PostgreSQL C++ 클라이언트
- [prometheus-cpp](https://github.com/jupp0r/prometheus-cpp)

### 매치메이킹/ELO
- [ELO Rating System](https://en.wikipedia.org/wiki/Elo_rating_system)
- [Microsoft TrueSkill](https://www.microsoft.com/en-us/research/publication/trueskill-a-bayesian-skill-rating-system/)

---

## Checkpoint A: 1v1 Duel Game

### v0.1.0 — Bootstrap (4일)

**목표**: 프로젝트 뼈대 구축 및 빌드 시스템 설정

**구현 파일**:
- `CMakeLists.txt` — C++17 표준, 컴파일 옵션
- `vcpkg.json` — boost, gtest, nlohmann-json 의존성
- `server/include/`, `server/src/`, `server/tests/`
- `deployments/docker/docker-compose.yml`

**핵심 코드 스니펫**:
```cmake
# CMakeLists.txt
cmake_minimum_required(VERSION 3.20)
project(pvp_server CXX)
set(CMAKE_CXX_STANDARD 17)
find_package(Boost REQUIRED COMPONENTS system thread)
```

```bash
GIT_AUTHOR_DATE="2025-01-01 20:30:00" GIT_COMMITTER_DATE="2025-01-01 20:30:00" git commit -m "feat: initialize CMake project with C++17 standard"
```

```bash
GIT_AUTHOR_DATE="2025-01-02 21:00:00" GIT_COMMITTER_DATE="2025-01-02 21:00:00" git commit -m "feat: add vcpkg manifest with Boost and GTest"
```

```bash
GIT_AUTHOR_DATE="2025-01-06 19:45:00" GIT_COMMITTER_DATE="2025-01-06 19:45:00" git commit -m "chore: setup directory structure"
```

```bash
GIT_AUTHOR_DATE="2025-01-07 20:15:00" GIT_COMMITTER_DATE="2025-01-07 20:15:00" git commit -m "chore: add Docker Compose for dev environment"
```

**검증**:
```bash
mkdir build && cd build
cmake ..
make
```

---

### v1.0.0 — Game Server Core (15일)

**목표**: 60 TPS 고정 틱 게임 루프 + WebSocket 서버

**구현 파일**:
- `include/pvpserver/core/config.h` — 환경변수 로더
- `include/pvpserver/core/game_loop.h` — 고정 시간 단계 루프
- `include/pvpserver/game/player_state.h` — 위치, 방향, 시퀀스
- `include/pvpserver/game/game_session.h` — 세션 관리
- `include/pvpserver/network/websocket_server.h` — Boost.Beast
- `include/pvpserver/storage/postgres_storage.h` — libpq 래퍼

**핵심 로직**:
```cpp
// game_loop.h
class GameLoop {
  void Run() {
    const double dt = 1.0 / 60.0; // 16.67ms
    auto next_tick = std::chrono::steady_clock::now();
    while (running_) {
      Tick(dt);
      next_tick += std::chrono::milliseconds(16);
      std::this_thread::sleep_until(next_tick);
    }
  }
};
```

```bash
GIT_AUTHOR_DATE="2025-01-08 20:00:00" GIT_COMMITTER_DATE="2025-01-08 20:00:00" git commit -m "feat: implement GameConfig with env loading"
```

```bash
GIT_AUTHOR_DATE="2025-01-13 21:30:00" GIT_COMMITTER_DATE="2025-01-13 21:30:00" git commit -m "feat: implement fixed timestep GameLoop at 60 TPS"
```

```bash
GIT_AUTHOR_DATE="2025-01-14 19:30:00" GIT_COMMITTER_DATE="2025-01-14 19:30:00" git commit -m "feat: add PlayerState struct"
```

```bash
GIT_AUTHOR_DATE="2025-01-20 20:45:00" GIT_COMMITTER_DATE="2025-01-20 20:45:00" git commit -m "feat: implement GameSession with player management"
```

```bash
GIT_AUTHOR_DATE="2025-01-21 19:15:00" GIT_COMMITTER_DATE="2025-01-21 19:15:00" git commit -m "feat: add WASD movement physics"
```

```bash
GIT_AUTHOR_DATE="2025-01-27 21:00:00" GIT_COMMITTER_DATE="2025-01-27 21:00:00" git commit -m "feat: implement async WebSocketServer with Boost.Beast"
```

```bash
GIT_AUTHOR_DATE="2025-01-28 20:30:00" GIT_COMMITTER_DATE="2025-01-28 20:30:00" git commit -m "feat: add client session management"
```

```bash
GIT_AUTHOR_DATE="2025-01-30 19:45:00" GIT_COMMITTER_DATE="2025-01-30 19:45:00" git commit -m "feat: implement PostgresStorage with libpq"
```

```bash
GIT_AUTHOR_DATE="2025-02-04 20:15:00" GIT_COMMITTER_DATE="2025-02-04 20:15:00" git commit -m "feat: add Prometheus metrics endpoint"
```

```bash
GIT_AUTHOR_DATE="2025-02-05 21:00:00" GIT_COMMITTER_DATE="2025-02-05 21:00:00" git commit -m "test: add GameLoop unit tests"
```

**검증**:
```bash
# 서버 실행
./build/pvp_server

# 다른 터미널에서 WebSocket 연결 테스트
wscat -c ws://localhost:8080
```

---

### v1.1.0 — Combat System (10일)

**목표**: 발사체 기반 전투 시스템

```bash
GIT_AUTHOR_DATE="2025-02-06 19:30:00" GIT_COMMITTER_DATE="2025-02-06 19:30:00" git commit -m "feat: implement HealthComponent"
```

```bash
GIT_AUTHOR_DATE="2025-02-10 20:00:00" GIT_COMMITTER_DATE="2025-02-10 20:00:00" git commit -m "feat: add Projectile class"
```

```bash
GIT_AUTHOR_DATE="2025-02-13 19:15:00" GIT_COMMITTER_DATE="2025-02-13 19:15:00" git commit -m "feat: implement fire input with cooldown"
```

```bash
GIT_AUTHOR_DATE="2025-02-17 21:00:00" GIT_COMMITTER_DATE="2025-02-17 21:00:00" git commit -m "feat: add circle-circle collision detection"
```

```bash
GIT_AUTHOR_DATE="2025-02-18 19:45:00" GIT_COMMITTER_DATE="2025-02-18 19:45:00" git commit -m "feat: implement damage resolution"
```

```bash
GIT_AUTHOR_DATE="2025-02-19 20:30:00" GIT_COMMITTER_DATE="2025-02-19 20:30:00" git commit -m "feat: add CombatLog"
```

```bash
GIT_AUTHOR_DATE="2025-02-24 19:00:00" GIT_COMMITTER_DATE="2025-02-24 19:00:00" git commit -m "test: add combat integration tests"
```

---

### v1.2.0 — Matchmaking (10일)

**목표**: ELO 기반 자동 매칭

```bash
GIT_AUTHOR_DATE="2025-02-25 20:00:00" GIT_COMMITTER_DATE="2025-02-25 20:00:00" git commit -m "feat: implement MatchRequest struct"
```

```bash
GIT_AUTHOR_DATE="2025-02-27 19:30:00" GIT_COMMITTER_DATE="2025-02-27 19:30:00" git commit -m "feat: add InMemoryMatchQueue with ELO buckets"
```

```bash
GIT_AUTHOR_DATE="2025-03-05 21:00:00" GIT_COMMITTER_DATE="2025-03-05 21:00:00" git commit -m "feat: implement Matchmaker algorithm"
```

```bash
GIT_AUTHOR_DATE="2025-03-06 19:15:00" GIT_COMMITTER_DATE="2025-03-06 19:15:00" git commit -m "feat: add dynamic tolerance expansion"
```

```bash
GIT_AUTHOR_DATE="2025-03-07 20:45:00" GIT_COMMITTER_DATE="2025-03-07 20:45:00" git commit -m "feat: implement MatchNotificationChannel"
```

```bash
GIT_AUTHOR_DATE="2025-03-10 19:30:00" GIT_COMMITTER_DATE="2025-03-10 19:30:00" git commit -m "feat: add matchmaking metrics"
```

```bash
GIT_AUTHOR_DATE="2025-03-11 20:00:00" GIT_COMMITTER_DATE="2025-03-11 20:00:00" git commit -m "test: add matchmaking performance tests"
```

---

### v1.3.0 — Stats & Leaderboard (8일)

**목표**: 통계 시스템 및 랭킹

```bash
GIT_AUTHOR_DATE="2025-03-12 19:15:00" GIT_COMMITTER_DATE="2025-03-12 19:15:00" git commit -m "feat: implement PlayerProfile"
```

```bash
GIT_AUTHOR_DATE="2025-03-13 20:30:00" GIT_COMMITTER_DATE="2025-03-13 20:30:00" git commit -m "feat: add MatchStatsCollector"
```

```bash
GIT_AUTHOR_DATE="2025-03-17 19:45:00" GIT_COMMITTER_DATE="2025-03-17 19:45:00" git commit -m "feat: implement EloRatingCalculator"
```

```bash
GIT_AUTHOR_DATE="2025-03-18 20:00:00" GIT_COMMITTER_DATE="2025-03-18 20:00:00" git commit -m "feat: add LeaderboardStore"
```

```bash
GIT_AUTHOR_DATE="2025-03-19 19:30:00" GIT_COMMITTER_DATE="2025-03-19 19:30:00" git commit -m "feat: implement ProfileService API"
```

```bash
GIT_AUTHOR_DATE="2025-03-20 21:00:00" GIT_COMMITTER_DATE="2025-03-20 21:00:00" git commit -m "test: add stats integration tests"
```

---

## 확장 v1.4.x: UDP 넷코드

### v1.4.0 — UDP Core (10일)

**목표**: UDP 기반 스냅샷/델타 동기화

```bash
GIT_AUTHOR_DATE="2025-03-24 20:15:00" GIT_COMMITTER_DATE="2025-03-24 20:15:00" git commit -m "feat: implement UdpSocket wrapper"
```

```bash
GIT_AUTHOR_DATE="2025-03-25 19:30:00" GIT_COMMITTER_DATE="2025-03-25 19:30:00" git commit -m "feat: add packet type system"
```

```bash
GIT_AUTHOR_DATE="2025-03-28 21:00:00" GIT_COMMITTER_DATE="2025-03-28 21:00:00" git commit -m "feat: implement SnapshotManager"
```

```bash
GIT_AUTHOR_DATE="2025-04-02 19:45:00" GIT_COMMITTER_DATE="2025-04-02 19:45:00" git commit -m "feat: add delta compression"
```

```bash
GIT_AUTHOR_DATE="2025-04-03 20:30:00" GIT_COMMITTER_DATE="2025-04-03 20:30:00" git commit -m "feat: implement UdpGameServer"
```

```bash
GIT_AUTHOR_DATE="2025-04-04 19:15:00" GIT_COMMITTER_DATE="2025-04-04 19:15:00" git commit -m "test: add UDP socket tests"
```

---

### v1.4.1 — Client Prediction (8일)

**목표**: 클라이언트 예측 및 서버 보정

```bash
GIT_AUTHOR_DATE="2025-04-09 21:00:00" GIT_COMMITTER_DATE="2025-04-09 21:00:00" git commit -m "feat: implement ClientPrediction"
```

```bash
GIT_AUTHOR_DATE="2025-04-11 19:30:00" GIT_COMMITTER_DATE="2025-04-11 19:30:00" git commit -m "feat: add Reconciliation system"
```

```bash
GIT_AUTHOR_DATE="2025-04-14 20:15:00" GIT_COMMITTER_DATE="2025-04-14 20:15:00" git commit -m "feat: implement InputBuffer"
```

```bash
GIT_AUTHOR_DATE="2025-04-16 19:45:00" GIT_COMMITTER_DATE="2025-04-16 19:45:00" git commit -m "feat: add LagCompensation"
```

```bash
GIT_AUTHOR_DATE="2025-04-17 21:00:00" GIT_COMMITTER_DATE="2025-04-17 21:00:00" git commit -m "test: add prediction tests"
```

---

### v1.4.2 — Observability (5일)

**목표**: 넷코드 모니터링

```bash
GIT_AUTHOR_DATE="2025-04-18 19:30:00" GIT_COMMITTER_DATE="2025-04-18 19:30:00" git commit -m "feat: implement UdpMetrics"
```

```bash
GIT_AUTHOR_DATE="2025-04-22 20:00:00" GIT_COMMITTER_DATE="2025-04-22 20:00:00" git commit -m "feat: add PacketSimulator"
```

```bash
GIT_AUTHOR_DATE="2025-04-23 19:15:00" GIT_COMMITTER_DATE="2025-04-23 19:15:00" git commit -m "perf: add load tests"
```

```bash
GIT_AUTHOR_DATE="2025-04-24 21:00:00" GIT_COMMITTER_DATE="2025-04-24 21:00:00" git commit -m "feat: create Grafana dashboards"
```

---

## 확장 v2.x: 분산 시스템

### v2.0.0 — Session Store (6일)

**목표**: Redis 기반 세션 저장소

```bash
GIT_AUTHOR_DATE="2025-04-25 19:30:00" GIT_COMMITTER_DATE="2025-04-25 19:30:00" git commit -m "feat: define SessionStore interface"
```

```bash
GIT_AUTHOR_DATE="2025-04-28 20:15:00" GIT_COMMITTER_DATE="2025-04-28 20:15:00" git commit -m "feat: implement InMemorySessionStore"
```

```bash
GIT_AUTHOR_DATE="2025-05-02 19:45:00" GIT_COMMITTER_DATE="2025-05-02 19:45:00" git commit -m "feat: add RedisSessionStore"
```

```bash
GIT_AUTHOR_DATE="2025-05-05 20:30:00" GIT_COMMITTER_DATE="2025-05-05 20:30:00" git commit -m "feat: implement session serialization"
```

---

### v2.0.1 — Load Balancer (6일)

**목표**: Consistent Hash 로드 밸런서

```bash
GIT_AUTHOR_DATE="2025-05-07 21:00:00" GIT_COMMITTER_DATE="2025-05-07 21:00:00" git commit -m "feat: implement ConsistentHashRing"
```

```bash
GIT_AUTHOR_DATE="2025-05-08 19:30:00" GIT_COMMITTER_DATE="2025-05-08 19:30:00" git commit -m "feat: add LoadBalancer"
```

```bash
GIT_AUTHOR_DATE="2025-05-09 20:00:00" GIT_COMMITTER_DATE="2025-05-09 20:00:00" git commit -m "feat: implement ServiceDiscovery"
```

```bash
GIT_AUTHOR_DATE="2025-05-12 19:15:00" GIT_COMMITTER_DATE="2025-05-12 19:15:00" git commit -m "feat: add HealthChecker"
```

---

### v2.0.2 — gRPC (4일)

**목표**: 서버 간 통신

```bash
GIT_AUTHOR_DATE="2025-05-14 20:30:00" GIT_COMMITTER_DATE="2025-05-14 20:30:00" git commit -m "feat: define gRPC protobuf schemas"
```

```bash
GIT_AUTHOR_DATE="2025-05-15 19:45:00" GIT_COMMITTER_DATE="2025-05-15 19:45:00" git commit -m "feat: implement MatchmakingService gRPC"
```

---

### v2.1.0 — Anti-Cheat (10일)

**목표**: 히트 검증 및 이상 탐지

```bash
GIT_AUTHOR_DATE="2025-05-16 20:00:00" GIT_COMMITTER_DATE="2025-05-16 20:00:00" git commit -m "feat: implement HitValidator"
```

```bash
GIT_AUTHOR_DATE="2025-05-19 19:30:00" GIT_COMMITTER_DATE="2025-05-19 19:30:00" git commit -m "feat: add MovementValidator"
```

```bash
GIT_AUTHOR_DATE="2025-05-21 21:00:00" GIT_COMMITTER_DATE="2025-05-21 21:00:00" git commit -m "feat: implement AnomalyDetector"
```

```bash
GIT_AUTHOR_DATE="2025-05-22 19:15:00" GIT_COMMITTER_DATE="2025-05-22 19:15:00" git commit -m "feat: add SuspicionSystem"
```

```bash
GIT_AUTHOR_DATE="2025-05-23 20:30:00" GIT_COMMITTER_DATE="2025-05-23 20:30:00" git commit -m "feat: implement ReplayRecorder"
```

```bash
GIT_AUTHOR_DATE="2025-05-26 19:45:00" GIT_COMMITTER_DATE="2025-05-26 19:45:00" git commit -m "feat: add BanService"
```

---

## 타임라인 요약

| 구간 | 버전 | 기간 | 커밋 수 | 비고 |
|------|------|------|--------|------|
| Checkpoint A | v0.1.0 ~ v1.3.0 | 1/1 ~ 3/7 | 34 | 기본 게임 서버 |
| UDP 넷코드 | v1.4.0 ~ v1.4.2 | 3/10 ~ 4/11 | 15 | 예측/보정 |
| 분산 시스템 | v2.0.0 ~ v2.1.0 | 4/14 ~ 5/23 | 16 | Redis, gRPC |
| **합계** | | **약 20주** | **65** | |

---

## 버전별 완료 체크리스트

### v0.1.0 완료 시
- [ ] `cmake ..` 실행 성공
- [ ] vcpkg 의존성 모두 설치됨
- [ ] `make` 빌드 성공
- [ ] Docker Compose PostgreSQL/Redis 정상 실행

### v1.0.0 완료 시
- [ ] GameLoop 60 TPS 유지 (틱 편차 < 1ms)
- [ ] WebSocket 클라이언트 연결 가능
- [ ] 플레이어 이동 입력 → 브로드캐스트 동작
- [ ] PostgreSQL에 이벤트 로그 저장 확인
- [ ] Prometheus `/metrics` 엔드포인트 응답

### v1.1.0 완료 시
- [ ] 발사체 생성 및 이동
- [ ] 충돌 시 데미지 적용
- [ ] HP 0 되면 사망 처리
- [ ] CombatLog 테이블에 히트 기록

### v1.2.0 완료 시
- [ ] 2명의 매치 요청 → 자동 매칭
- [ ] ELO 차이 큰 플레이어는 매칭 안됨
- [ ] 5초마다 허용 오차 확장 동작
- [ ] 매칭 성공 시 GameSession 생성

### v1.3.0 완료 시
- [ ] 매치 종료 후 ELO 업데이트
- [ ] 리더보드 Top 100 조회
- [ ] PlayerProfile API 정상 응답

### v1.4.0 완료 시
- [ ] UDP 패킷 송수신 정상
- [ ] 델타 압축 적용 (패킷 크기 80% 감소)
- [ ] 스냅샷 히스토리 버퍼링

### v1.4.1 완료 시
- [ ] 클라이언트 로컬 예측 스무스
- [ ] 서버 보정 시 끊김 없음
- [ ] 200ms 지연 환경에서도 플레이 가능

### v1.4.2 완료 시
- [ ] Grafana 대시보드에서 RTT, 패킷 손실률 확인
- [ ] PacketSimulator로 지연/손실 테스트
- [ ] 부하 테스트 10 동시 매치 안정적

### v2.0.0 완료 시
- [ ] Redis에 세션 저장/복원
- [ ] 서버 재시작 후 세션 복구

### v2.0.1 완료 시
- [ ] ConsistentHash로 플레이어 분산
- [ ] 서버 추가/제거 시 최소 재배치

### v2.0.2 완료 시
- [ ] gRPC 서버간 매치메이킹 요청 성공

### v2.1.0 완료 시
- [ ] 불가능한 히트 차단
- [ ] 속도핵 의심 플레이어 검출
- [ ] 의심 레벨 3 이상 시 자동 밴

---

## 디버깅 팁

### 빌드 에러
```bash
# vcpkg 경로 확인
echo $VCPKG_ROOT

# CMake 캐시 삭제 후 재시도
rm -rf build && mkdir build && cd build
cmake -DCMAKE_TOOLCHAIN_FILE=$VCPKG_ROOT/scripts/buildsystems/vcpkg.cmake ..
```

### WebSocket 연결 실패
```bash
# 포트 확인
lsof -i :8080

# 방화벽 체크 (macOS)
sudo pfctl -s rules
```

### PostgreSQL 연결 에러
```bash
# Docker 로그 확인
docker logs pvp-postgres

# 연결 테스트
psql -h localhost -U postgres -d pvpdb
```

### 틱 레이트 불안정
```cpp
// 프로파일링 추가
auto start = std::chrono::steady_clock::now();
Tick(dt);
auto end = std::chrono::steady_clock::now();
auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
std::cout << "Tick duration: " << duration.count() << "μs\n";
```
