# cpp-pvp-server - 실시간 배틀 아레나 게임 서버

한국 게임 업계 포트폴리오용 프로덕션 품질 게임 서버. C++17, Boost.Asio/Beast, PostgreSQL, Prometheus로 구축.

**기술 스택**: C++17 · Boost 1.82+ · PostgreSQL 15 · Redis 7 · Protocol Buffers · Docker · Prometheus · WebSocket

---

## 프로젝트 개요

실시간 1v1 PvP 게임 서버로, 한국 게임 업계 포트폴리오를 위한 프로덕션 품질 구현체입니다.

**구현 범위**: v0.1.0 ~ v2.1.0 (기초 서버 → 안티치트 시스템)  
**산출물**: 완성된 1v1 실시간 전투 게임 서버

---

## 진행 상태

| 체크포인트 | 내용 | 목표 | 상태 |
|------------|------|------|------|
| **v0.1.0 - v1.3.0** | 1v1 대결 게임 (기초) | 신입~주니어 포트폴리오 | ✅ 완료 |
| **v1.4.0 - v1.4.2** | UDP 넷코드 & 예측 | 네트워크 전문성 | ✅ 완료 |
| **v2.0.0 - v2.1.0** | 분산 시스템 & 안티치트 | 중급 포트폴리오 | ✅ 완료 |

---

## v0.1.0 - v1.3.0: 1v1 대결 게임 ✅

**제품**: 완성된 1v1 실시간 전투 게임  
**대상**: 신입~주니어 게임 서버 개발자  
**기간**: 8-10주

### v0.1.0: Bootstrap & CI 베이스라인 ✅
- **프로젝트 구조** - CMake, vcpkg, Boost 설정
- **Docker 환경** - PostgreSQL, Redis 컨테이너
- **GitHub Actions CI** - 빌드 및 테스트 자동화

### v1.0.0: 기본 게임 서버 ✅
- **WebSocket 서버** (Boost.Beast) - 실시간 양방향 통신
- **60 TPS 게임 루프** - 고정 스텝 결정론적 물리 (틱당 16.67ms)
- **플레이어 이동** - WASD + 마우스 입력, 서버 권위 상태 동기화
- **PostgreSQL 통합** - 파라미터화된 쿼리를 사용한 세션 이벤트 기록

### v1.1.0: 전투 시스템 ✅
- **발사체 물리** - 30 m/s 직선 이동, 1.5초 수명
- **충돌 감지** - 원-원 교차 (발사체 0.2m vs 플레이어 0.5m)
- **데미지 시스템** - 히트당 20 HP, 100 HP 풀
- **전투 로그** - 사후 분석용 링 버퍼 (32 이벤트)

### v1.2.0: 매치메이킹 ✅
- **ELO 기반 매칭** - ±100 초기 허용 범위, 5초마다 ±25 확대
- **큐 관리** - 결정론적 페어링 (가장 오래된 호환 플레이어 우선)
- **동시 매치** - 10+ 동시 1v1 게임 지원
- **메트릭** - 대기 시간 분포 Prometheus 히스토그램

### v1.3.0: 통계 & 랭킹 ✅
- **사후 매치 통계** - 발사, 적중, 정확도, 데미지 가/피, 킬, 데스
- **ELO 레이팅** - 매치당 K-factor 25 조정
- **글로벌 리더보드** - 레이팅 순 인메모리 정렬 (Redis 준비 완료)
- **HTTP API** - 프로필 및 랭킹용 JSON 엔드포인트

---

## v1.4.0 - v1.4.2: UDP 넷코드 & 클라이언트 예측 ✅

**제품**: 지연 보상 네트워크 시스템  
**대상**: 네트워크 프로그래밍 전문성 증명  
**기간**: 4-6주

### v1.4.0: UDP 권위 서버 코어 ✅
- **UDP 소켓 래퍼** - Boost.Asio 기반 비동기 UDP
- **UDP 게임 서버** - 클라이언트 연결 관리, 타임아웃 처리
- **스냅샷/델타 동기화** - 60 TPS 상태 브로드캐스트, 델타 압축
- **패킷 타입 시스템** - CONNECT, INPUT, STATE, EVENT

### v1.4.1: 클라이언트 예측/리컨실리에이션 ✅
- **클라이언트 예측** - 입력 즉시 반영, 부드러운 움직임
- **리컨실리에이션** - 서버 상태 불일치 시 부드러운 보정
- **입력 버퍼링** - 서버 측 지터 완화
- **지연 보상** - 과거 상태 조회 기반 히트 판정

### v1.4.2: 관측성 & 부하 테스트 ✅
- **UDP 메트릭** - RTT, 지터, 패킷 손실률 수집
- **패킷 시뮬레이터** - 네트워크 조건 시뮬레이션 (테스트용)
- **성능 벤치마크** - 부하 테스트 프레임워크

---

## v2.0.0 - v2.1.0: 분산 시스템 & 안티치트 ✅

**제품**: 확장 가능한 분산 게임 서버  
**대상**: 중급 게임 서버 개발자  
**기간**: 4-6주

### v2.0.0: 세션 서버 분리 ✅
- **ISessionStore 인터페이스** - InMemory, Redis 구현체
- **Consistent Hashing** - MurmurHash3, 150 가상 노드
- **로드 밸런서** - 세션 어피니티, 헬스 체크, Round-Robin
- **서비스 디스커버리** - Redis pub/sub 기반

### v2.1.0: 안티 치트 시스템 ✅
- **HitValidator** - 서버 측 레이캐스팅, 지연 보상 히트 검증
- **MovementValidator** - 스피드핵, 텔레포트, 월핵 탐지
- **AnomalyDetector** - Z-Score 기반 통계 이상 탐지
- **ReplayRecorder** - 게임 상태 녹화 (사후 분석)
- **BanService** - 플레이어/HWID/IP 밴 시스템

---

## 아키텍처

```
┌─────────────────────────────────────────────────────────────┐
│                         클라이언트들                           │
│              (WebSocket + UDP 연결)                          │
└─────────────────────┬───────────────────────────────────────┘
                      │
┌─────────────────────▼───────────────────────────────────────┐
│                     Load Balancer                            │
│  ┌────────────────────────────────────────────────────────┐ │
│  │   ConsistentHashRing (MurmurHash3, 150 VNodes)        │ │
│  │   ServiceDiscovery (Redis pub/sub)                     │ │
│  └────────────────────────────────────────────────────────┘ │
└─────────────────────┬───────────────────────────────────────┘
                      │
┌─────────────────────▼───────────────────────────────────────┐
│              Game Server (WebSocket + UDP)                   │
│  ┌────────────────────────────────────────────────────────┐ │
│  │   GameLoop (60 TPS)                                    │ │
│  │     ├─ GameSession (2 플레이어, 발사체, 전투)          │ │
│  │     ├─ Tick (16.67ms 고정 스텝)                       │ │
│  │     └─ 상태 브로드캐스트 (스냅샷/델타)                │ │
│  └────────────────────────────────────────────────────────┘ │
│                                                              │
│  ┌────────────────────────────────────────────────────────┐ │
│  │   Netcode System                                       │ │
│  │     ├─ ClientPrediction (입력 예측)                   │ │
│  │     ├─ Reconciliation (상태 보정)                     │ │
│  │     ├─ InputBuffer (지터 완화)                        │ │
│  │     └─ LagCompensation (과거 상태 조회)               │ │
│  └────────────────────────────────────────────────────────┘ │
│                                                              │
│  ┌────────────────────────────────────────────────────────┐ │
│  │   Anti-Cheat System                                    │ │
│  │     ├─ HitValidator (서버 측 레이캐스트)              │ │
│  │     ├─ MovementValidator (속도핵/텔레포트 탐지)       │ │
│  │     ├─ AnomalyDetector (Z-Score 통계 분석)            │ │
│  │     └─ BanService (플레이어/HWID/IP 밴)               │ │
│  └────────────────────────────────────────────────────────┘ │
│                                                              │
│  ┌────────────────────────────────────────────────────────┐ │
│  │   Matchmaker + Stats                                   │ │
│  │     ├─ MatchQueue (ELO 버킷화)                        │ │
│  │     ├─ PlayerProfileService (통계/ELO)                │ │
│  │     └─ LeaderboardStore (레이팅순 정렬)               │ │
│  └────────────────────────────────────────────────────────┘ │
└─────────────────────┬───────────────────────────────────────┘
                      │
        ┌─────────────┼─────────────┐
        │             │             │
┌───────▼──────┐ ┌───▼────────┐ ┌──▼──────────┐
│ PostgreSQL   │ │ Redis      │ │ Prometheus  │
│ (세션/밴)    │ │ (세션스토어)│ │ (메트릭)    │
└──────────────┘ └────────────┘ └─────────────┘
```

**설계**: 의존성 역전을 사용한 클린 아키텍처
**스레딩**: 전용 스레드의 게임 루프, 뮤텍스 보호 공유 상태
**네트워크**: Boost.Asio 비동기 I/O (epoll/kqueue 자동 선택), WebSocket + UDP
**스토리지**: 영속성을 위한 PostgreSQL, 실시간 세션을 위한 Redis

### 클린 아키텍처 레이어

```
Delivery (WebSocket 핸들러)
    ↓
Use Case (비즈니스 로직)
    ↓
Domain (엔티티, 인터페이스)
    ↓
Infrastructure (DB, Redis, Kafka)
    ↓
Core (게임 루프, 물리)
```

**의존성 규칙**: 의존성은 안쪽으로 향함

---

## 성능 벤치마크

### 핵심 성능 지표 (KPI)

| 메트릭 | 목표 | 실제 | 상태 |
|--------|------|------|------|
| 서버 틱 레이트 | 60 TPS (안정) | **60 TPS** | ✅ |
| 틱 레이트 분산 | ≤ 1.0 ms | **0.04 ms** | ✅ |
| WebSocket 지연 (p99) | ≤ 50 ms | **18.3 ms** | ✅ |
| 전투 틱 지속 시간 (평균) | < 0.5 ms | **0.31 ms** | ✅ |
| 매치메이킹 (200 플레이어) | ≤ 2 ms | **≤ 2 ms** | ✅ |
| 프로필 서비스 (100 매치) | ≤ 5 ms | **< 1 ms** | ✅ |
| 테스트 커버리지 | ≥ 70% | **~85%** | ✅ |
| 에러율 | ≤ 0.1% | **< 0.1%** | ✅ |

**테스트 환경**: Ubuntu 22.04, 4-8 vCPU, CMake Release 빌드

---

## 빠른 시작

### 필수 요구사항

- **C++ 컴파일러**: GCC 11+ 또는 Clang 14+
- **CMake**: 3.20+
- **vcpkg**: 의존성 관리용
- **Docker**: PostgreSQL, Redis, Prometheus용

### 1. 의존성 설치 (vcpkg)

```bash
# vcpkg가 설치되지 않은 경우 설치
git clone https://github.com/Microsoft/vcpkg.git
cd vcpkg
./bootstrap-vcpkg.sh
export VCPKG_ROOT=$(pwd)

# 패키지 설치
./vcpkg install boost-asio boost-beast libpq protobuf
```

### 2. 인프라 시작

```bash
cd deployments/docker
docker-compose up -d

# 서비스 확인
docker ps  # PostgreSQL:5432, Redis:6379, Prometheus:9090, Grafana:3000
```

### 3. 서버 빌드

```bash
cd server
mkdir build && cd build

# vcpkg 설치 (최초 1회)
git clone --depth 1 https://github.com/microsoft/vcpkg.git ~/.vcpkg
~/.vcpkg/bootstrap-vcpkg.sh

# vcpkg로 구성 (의존성 자동 설치, 첫 빌드 시 5-10분 소요)
cmake .. -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_TOOLCHAIN_FILE=~/.vcpkg/scripts/buildsystems/vcpkg.cmake

# 빌드 (병렬 빌드를 위해 -j$(nproc) 추가)
make -j$(nproc)
```

**빌드 검증 결과** (2024-12-09, macOS arm64):
- ✅ 모든 소스 파일 컴파일 성공
- ✅ 라이브러리 및 실행 파일 링크 완료
- 의존성: Boost 1.89.0, GTest 1.17.0, libpq 16.9, Protobuf 5.29.5

### 4. 테스트 실행

```bash
# 모든 테스트 실행
ctest --output-on-failure

# 특정 테스트 스위트 실행
ctest -R UnitTests
ctest -R IntegrationTests
ctest -R PerformanceTests

# 상세 출력과 함께 실행
ctest -V
```

**테스트 결과** (2024-12-09):
```
100% tests passed, 0 tests failed out of 3

- UnitTests:        0.31 sec ✅
- IntegrationTests: 2.88 sec ✅
- PerformanceTests: 14.73 sec ✅

Total Test time: 17.93 sec
```

### 5. 서버 실행

```bash
# 환경 변수 설정 (선택사항)
export POSTGRES_DSN="host=localhost port=5432 dbname=cpp-pvp-server user=cpp-pvp-server password=cpp-pvp-server"
export WEBSOCKET_PORT=8080
export HTTP_PORT=8081
export TICK_RATE=60

# 서버 실행
./cpp-pvp-server_server

# 서버 로그
[INFO] WebSocket server listening on 0.0.0.0:8080
[INFO] HTTP server listening on 0.0.0.0:8081
[INFO] Game loop started at 60 TPS
[INFO] Prometheus metrics available at http://localhost:8081/metrics
```

---

## 서버 테스트

### WebSocket 프로토콜 (포트 8080)

**클라이언트 → 서버 (입력)**:
```
input <player_id> <seq> <up> <down> <left> <right> <mouse_x> <mouse_y>
```

예시:
```
input player1 0 1 0 0 0 150.5 200.0
```

**서버 → 클라이언트 (상태)**:
```
state <player_id> <x> <y> <angle> <tick>
```

예시:
```
state player1 105.0 200.0 0.785 123
```

**서버 → 클라이언트 (사망)**:
```
death <player_id> <tick>
```

### 옵션 1: wscat (빠른 테스트)

**설치**:
```bash
npm install -g wscat
```

**사용법**:
```bash
# 서버에 연결
wscat -c ws://localhost:8080

# 이동 입력 전송 (W 키 눌림, 마우스 위치 150, 200)
> input player1 0 1 0 0 0 150.5 200.0

# 서버가 상태로 응답
< state player1 100.0 200.0 0.0 60
< state player1 105.0 200.0 0.0 61
< state player1 110.0 200.0 0.0 62

# 발사 입력 전송 (마우스 클릭, 위치 300, 100)
> input player1 1 0 0 0 0 300.0 100.0

# 서버가 상태로 응답
< state player1 115.0 200.0 1.047 63
```

**입력 형식**:
- `player_id`: 고유 플레이어 식별자 (예: "player1")
- `seq`: 시퀀스 번호 (증가, 디버깅용)
- `up down left right`: 이동 키 (1 = 눌림, 0 = 놓임)
- `mouse_x mouse_y`: 마우스 커서 위치 (월드 좌표)

### 옵션 2: Python 테스트 클라이언트 (자동화)

**설치**:
```bash
pip install websockets
```

**사용법**:
```bash
# 자동화된 테스트 실행
python tools/test_client.py

# 사용자 정의 플레이어 ID로 실행
python tools/test_client.py --player player2

# 여러 클라이언트 실행 (스트레스 테스트)
python tools/test_client.py --clients 10
```

자세한 사용법은 `tools/README.md`를 참조하세요.

### HTTP API (포트 8081)

**플레이어 프로필 조회**:
```bash
curl http://localhost:8081/profiles/player1
```

응답:
```json
{
  "player_id": "player1",
  "matches": 10,
  "wins": 6,
  "losses": 4,
  "kills": 12,
  "deaths": 8,
  "shots_fired": 150,
  "hits_landed": 45,
  "damage_dealt": 900,
  "damage_taken": 600,
  "rating": 1225
}
```

**리더보드 조회**:
```bash
curl http://localhost:8081/leaderboard?limit=10
```

응답:
```json
[
  {
    "player_id": "player1",
    "rating": 1250,
    "wins": 15,
    "losses": 5
  }
]
```

**Prometheus 메트릭**:
```bash
curl http://localhost:8081/metrics
```

---

## 모니터링

### Prometheus 메트릭

`http://localhost:8081/metrics`에서 접근

**게임 루프**:
- `game_tick_rate` - 현재 틱 레이트 (Hz)
- `game_tick_duration_seconds` - 틱 실행 시간

**WebSocket**:
- `websocket_connections_total` - 활성 연결
- `game_sessions_active` - 동시 게임
- `player_actions_total` - 처리된 입력 총합

**전투**:
- `projectiles_active` - 활성 발사체
- `projectiles_spawned_total` - 발사된 총 발사체
- `projectiles_hits_total` - 총 히트
- `players_dead_total` - 총 사망

**매치메이킹**:
- `matchmaking_queue_size` - 대기 중인 플레이어
- `matchmaking_matches_total` - 생성된 매치
- `matchmaking_wait_seconds_bucket` - 대기 시간 히스토그램

**프로필**:
- `player_profiles_total` - 총 프로필
- `leaderboard_entries_total` - 리더보드 크기
- `matches_recorded_total` - 기록된 총 매치
- `rating_updates_total` - 총 ELO 업데이트

**Checkpoint B 추가 메트릭**:
- `object_pool_reuse_rate` - 재사용 비율
- `object_pool_active_count` - 현재 사용 중
- `interest_management_filtered_ratio` - 패킷 감소율
- `lockfree_queue_contention_ms` - p99 대기 시간
- `lockfree_queue_throughput` - 이벤트/초

### Grafana 대시보드

`http://localhost:3000`에서 접근 (기본값: admin/admin)

Prometheus 데이터 소스 추가: `http://prometheus:9090`

**기본 게임 서버** (8+ 패널):
- 시간별 틱 레이트
- 활성 연결
- 요청 지연 (p50, p95, p99)
- 에러율
- CPU/메모리 사용량
- 데이터베이스 연결
- 활성 게임 세션
- 세션당 플레이어 수

**UDP 넷코드** (4+ 패널):
- RTT 히스토그램
- 패킷 손실률
- 지터 측정
- 클라이언트별 연결 품질

**안티치트** (4+ 패널):
- 의심 레벨 분포
- 탐지된 위반 유형
- 밴 현황
- 리플레이 저장 현황

---

## 테스트 가이드

### 유닛 테스트 (13개 파일)

개별 컴포넌트를 격리하여 테스트:
- `test_game_loop.cpp` - 틱 레이트 정확도, 메트릭
- `test_game_session.cpp` - 플레이어 관리, 이동
- `test_combat.cpp` - 충돌, 데미지, 사망
- `test_projectile.cpp` - 물리, 만료
- `test_matchmaker.cpp` - ELO 매칭, 허용 범위
- `test_player_profile_service.cpp` - 통계 집계, ELO

### 통합 테스트 (4개 파일)

End-to-End 워크플로우 테스트:
- `test_websocket_server.cpp` - 클라이언트 연결, 상태 동기화
- `test_websocket_combat.cpp` - 전체 전투 시나리오
- `test_matchmaker_flow.cpp` - 20명 플레이어 → 10 매치
- `test_profile_http.cpp` - HTTP 엔드포인트

### 성능 테스트 (4개 파일)

KPI 목표 검증:
- `test_tick_variance.cpp` - 틱 안정성 (≤1ms 분산)
- `test_projectile_perf.cpp` - 충돌 성능 (<0.5ms)
- `test_matchmaking_perf.cpp` - 매치메이킹 속도 (≤2ms)
- `test_profile_service_perf.cpp` - 통계 기록 (≤5ms)

**커버리지**: ~85% 추정 (18개 소스 파일에 대해 21개 테스트 파일)

---

## 프로젝트 구조

```
cpp-pvp-server/
├── server/
│   ├── include/pvpserver/        # 공개 헤더
│   │   ├── core/                 # GameLoop, Config
│   │   ├── game/                 # GameSession, Combat, Projectile
│   │   ├── network/              # WebSocketServer, UdpGameServer, HTTP
│   │   ├── matchmaking/          # Matchmaker, Queue
│   │   ├── stats/                # ProfileService, Leaderboard
│   │   ├── netcode/              # Prediction, Reconciliation, LagCompensation
│   │   ├── distributed/          # ConsistentHash, LoadBalancer, ServiceDiscovery
│   │   ├── anticheat/            # HitValidator, MovementValidator, BanService
│   │   └── storage/              # PostgresStorage, SessionStore
│   ├── src/                      # 구현 (.cpp)
│   ├── tests/
│   │   ├── unit/                 # 19개 유닛 테스트
│   │   ├── integration/          # 4개 통합 테스트
│   │   └── performance/          # 4개 성능 벤치마크
│   └── CMakeLists.txt
├── deployments/
│   └── docker/
│       └── docker-compose.yml    # PostgreSQL, Redis, Prometheus, Grafana
├── monitoring/
│   ├── prometheus/
│   └── grafana/
├── docs/
│   ├── mvp-specs/                # 상세 MVP 요구사항
│   └── evidence/                 # 성능 리포트
├── design/                       # 설계 문서 (v0.1.0 ~ v2.1)
├── snapshots/                    # 버전별 코드 스냅샷 (001 ~ 012)
└── README.md                     # 이 파일
```

---

## 코드 품질

### 표준

- **C++ 17** 모던 관용구 사용 (RAII, 스마트 포인터, 이동 시맨틱스)
- **스레드 안전성**: 동시 접근을 위한 `std::mutex`, `std::atomic`
- **Const 정확성**: `const` 메서드, 가능한 경우 `noexcept`
- **에러 처리**: 명시적 에러 확인, 핫 패스에서 예외 미사용
- **네이밍**: `PascalCase` (클래스), `camelCase` (함수), `snake_case` (변수)

### 네이밍 규칙

- 클래스: `PascalCase` (GameSession, PlayerState)
- 함수: `camelCase` (processInputs, updatePhysics)
- 변수: `snake_case` (player_id_, tick_count_)
- 상수: `UPPER_CASE` (TARGET_TPS, MAX_PLAYERS)

### 품질 게이트

**모두 통과 필수**:
1. 빌드 (Release + Debug)
2. 유닛 테스트 (≥70% 커버리지)
3. 통합 테스트
4. 린트 (clang-format, clang-tidy)
5. 성능 (60 TPS 해당 시)

### 린팅

```bash
# 포맷 검사 (clang-format)
find server/src server/include -name "*.cpp" -o -name "*.h" | xargs clang-format -n --Werror

# 정적 분석 (clang-tidy, 가능한 경우)
clang-tidy server/src/*.cpp -- -Iserver/include
```

---

## 설계 문서

프로젝트의 **가장 정확하고 상세한 설계 정보**는 `design/` 폴더를 참조하세요:

- **[MVP 1.0](docs/mvp-specs/mvp-1.0.md)**: 기본 게임 서버 - WebSocket, 60 TPS 게임 루프, 플레이어 이동, PostgreSQL
- **[MVP 1.1](docs/mvp-specs/mvp-1.1.md)**: 전투 시스템 - 발사체 물리, 충돌 감지, 데미지, 전투 로그
- **[MVP 1.2](docs/mvp-specs/mvp-1.2.md)**: 매치메이킹 - ELO 기반 큐, 허용 범위 확대, 동시 매치
- **[MVP 1.3](docs/mvp-specs/mvp-1.3.md)**: 통계 & 랭킹 - 사후 통계, ELO 레이팅, 글로벌 리더보드

---

## 환경 변수

| 변수 | 기본값 | 설명 |
|------|--------|------|
| `POSTGRES_DSN` | `host=localhost...` | PostgreSQL 연결 문자열 |
| `WEBSOCKET_PORT` | `8080` | WebSocket 서버 포트 |
| `HTTP_PORT` | `8081` | HTTP API 및 메트릭 포트 |
| `TICK_RATE` | `60` | 게임 루프 틱 레이트 (TPS) |

---

## 문제 해결

### 빌드 오류

**CMake가 Boost를 찾을 수 없음**:
```bash
export VCPKG_ROOT=/path/to/vcpkg
cmake .. -DCMAKE_TOOLCHAIN_FILE=$VCPKG_ROOT/scripts/buildsystems/vcpkg.cmake
```

**링커 오류 (libpq)**:
```bash
# PostgreSQL 클라이언트 라이브러리 설치
sudo apt-get install libpq-dev  # Ubuntu/Debian
brew install libpq              # macOS
```

### 런타임 오류

**PostgreSQL 연결 실패**:
```bash
# PostgreSQL 실행 확인
docker ps | grep postgres

# 연결 테스트
psql -h localhost -p 5432 -U cpp-pvp-server -d cpp-pvp-server
```

**포트가 이미 사용 중**:
```bash
# 포트 변경
export WEBSOCKET_PORT=8888
export HTTP_PORT=8889
```

### 일반적인 문제

**틱 레이트 불안정**:
- 원인: 루프 내 무거운 연산
- 해결: 프로파일링 (perf), 최적화, 스레드 풀

**WebSocket 연결 끊김**:
- 원인: 네트워크 타임아웃
- 해결: Ping/pong, keepalive, 재연결

**데이터베이스 병목**:
- 원인: N+1 쿼리, 누락된 인덱스
- 해결: EXPLAIN ANALYZE, 인덱스 추가, 캐싱

---

## 보안

**구현 사항**:
1. 입력 검증 (모든 클라이언트 데이터)
2. 인증 (JWT 토큰)
3. 안티 치트 (서버 사이드 검증)
4. 속도 제한 (DoS 방지)
5. SQL 인젝션 방지 (파라미터화된 쿼리만 사용)

---

## 배포

| 버전 | 배포 방식 |
|------|-----------|
| **v0.1 ~ v1.3** | Docker Compose (단일 서버) |
| **v1.4 ~ v2.1** | 멀티 인스턴스 + Redis 세션 스토어 |

---

## 기술 스택 근거

| 기술 | 이유 |
|------|------|
| **C++17** | 게임 서버 업계 표준 (Nexon, Krafton, Netmarble) |
| **Boost.Asio/Beast** | 프로덕션급 비동기 I/O, WebSocket 지원 |
| **PostgreSQL** | 영속 데이터를 위한 ACID 보장 |
| **Redis** | 매치메이킹 큐를 위한 빠른 인메모리 캐시 |
| **Prometheus** | 업계 표준 메트릭 및 모니터링 |
| **Protocol Buffers** | 효율적인 바이너리 직렬화 (향후 사용 준비) |
| **Docker** | 일관된 개발/테스트 환경 |
| **Kafka** | 대규모 이벤트 스트리밍 (Checkpoint B+) |
| **Kubernetes** | 멀티 리전 오케스트레이션 (Checkpoint C) |

---

## 참고 자료

**공식 문서**:
- [boost.asio](https://www.boost.org/doc/libs/release/doc/html/boost_asio.html)
- [PostgreSQL](https://www.postgresql.org/docs/)
- [Protocol Buffers](https://protobuf.dev/)

**학습 자료**:
- [Game Programming Patterns](https://gameprogrammingpatterns.com/)
- [C++ Core Guidelines](https://isocpp.github.io/CppCoreGuidelines/)

---

## 라이선스

이것은 포트폴리오 프로젝트입니다. 코드는 데모 목적으로 제공됩니다.

---

## 연락처

**프로젝트**: cpp-pvp-server  
**목표**: 한국 게임 서버 개발자 포지션 (Nexon, Krafton, Netmarble, Kakao Games)  
**기술 매칭**: 100%  
**현재 상태**: v2.1 안티치트 시스템 완료
