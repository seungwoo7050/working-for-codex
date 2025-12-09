# cpp-pvp-server

C++17로 만든 1v1 실시간 대전 게임 서버.  
60 TPS 권위 서버 + UDP 넷코드 + 분산 세션 지원.

## 왜 만들었나

게임 서버 개발자 취업 준비하면서, 직접 처음부터 만들어보고 싶었음.  
유니티나 언리얼 말고 순수 C++로 네트워크부터 게임 로직까지.

## 기술 스택

- **언어**: C++17
- **빌드**: CMake 3.20+, vcpkg
- **네트워크**: Boost.Asio, Boost.Beast (WebSocket), UDP
- **DB**: PostgreSQL, Redis
- **직렬화**: Protocol Buffers, JSON
- **서버간 통신**: gRPC
- **모니터링**: Prometheus, Grafana

## 기능

### 기본 (Checkpoint A)
- 60 TPS 고정 틱 게임 루프
- WebSocket 기반 실시간 통신
- 발사체/충돌 전투 시스템
- ELO 기반 매치메이킹 (동적 허용 오차)
- 리더보드, 전적 통계

### UDP 넷코드 (v1.4.x)
- 스냅샷/델타 압축
- 클라이언트 예측 + 서버 리컨실리에이션
- 래그 보상 (200ms까지)
- 입력 버퍼링

### 분산 시스템 (v2.x)
- Redis 기반 분산 세션
- Consistent Hash 로드 밸런싱
- gRPC 서버간 통신
- 안티치트 (히트 검증, 이상 탐지)

## 프로젝트 구조

```
server/
├── include/pvpserver/
│   ├── core/          # GameLoop, Config
│   ├── game/          # GameSession, Combat, Projectile
│   ├── network/       # WebSocket, UDP
│   ├── matchmaking/   # Matchmaker, Queue
│   ├── stats/         # Profile, Leaderboard
│   ├── netcode/       # Prediction, Reconciliation
│   ├── distributed/   # ConsistentHash, LoadBalancer
│   ├── anticheat/     # HitValidator, AnomalyDetector
│   └── storage/       # Postgres, Redis
├── src/
└── tests/
```

## 빌드

```bash
# vcpkg 설치 (처음 한번)
git clone https://github.com/microsoft/vcpkg.git
./vcpkg/bootstrap-vcpkg.sh
export VCPKG_ROOT=$(pwd)/vcpkg

# 빌드
cd server
mkdir build && cd build
cmake -DCMAKE_TOOLCHAIN_FILE=$VCPKG_ROOT/scripts/buildsystems/vcpkg.cmake ..
make -j$(nproc)
```

## 실행

```bash
# 인프라 띄우기
docker-compose -f deployments/docker/docker-compose.yml up -d

# 서버 실행
./build/pvp_server

# 테스트
ctest --output-on-failure
```

## 환경변수

| 변수 | 기본값 | 설명 |
|------|--------|------|
| `PVP_PORT` | 8080 | WebSocket 포트 |
| `PVP_UDP_PORT` | 9000 | UDP 포트 |
| `PVP_DB_HOST` | localhost | PostgreSQL 호스트 |
| `PVP_REDIS_HOST` | localhost | Redis 호스트 |
| `PVP_TICK_RATE` | 60 | 초당 틱 수 |

## 성능

로컬 벤치마크 (M1 Mac):

| 항목 | 결과 |
|------|------|
| 틱 레이트 | 60.0 TPS (±0.1) |
| 틱 처리 시간 | p99 < 2ms |
| 매치메이킹 (200명) | < 2ms |
| 델타 압축률 | ~80% |

## 참고 자료

- [Gaffer On Games](https://gafferongames.com/) - 게임 네트워킹 필독
- [Valve Source Networking](https://developer.valvesoftware.com/wiki/Source_Multiplayer_Networking)
- [Overwatch Netcode GDC](https://www.youtube.com/watch?v=W3aieHjyNvw)

## TODO

- [ ] 클라이언트 구현 (SDL2 or SFML)
- [ ] 3v3 모드
- [ ] 리플레이 뷰어
- [ ] Kubernetes 배포

## License

MIT
