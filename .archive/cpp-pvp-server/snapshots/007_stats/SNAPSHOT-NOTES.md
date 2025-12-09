# 스냅샷 007_stats (v1.3.0)

> 통계 & 순위 시스템 완료 시점

## 대응 버전
- **버전**: v1.3.0
- **패치 범위**: 1.3.1 ~ 1.3.4

## 패치 내용
| 패치 ID | 작업 | 핵심 파일 |
|---------|------|----------|
| 1.3.1 | 통계 집계 서비스 | `stats/match_stats.*`, `stats/player_profile_service.*` |
| 1.3.2 | 랭킹 API | `stats/leaderboard_store.*` |
| 1.3.3 | Prometheus 메트릭 | `network/metrics_http_server.*` |
| 1.3.4 | Grafana 대시보드 | `monitoring/grafana/` |

## 디렉토리 구조
```
server/
├── CMakeLists.txt
├── vcpkg.json
├── include/pvpserver/
│   ├── core/
│   │   ├── config.h
│   │   └── game_loop.h
│   ├── game/
│   │   ├── combat.h
│   │   ├── game_session.h
│   │   ├── movement.h
│   │   ├── player_state.h
│   │   └── projectile.h
│   ├── matchmaking/
│   │   ├── match.h
│   │   ├── match_notification_channel.h
│   │   ├── match_queue.h
│   │   ├── match_request.h
│   │   └── matchmaker.h
│   ├── network/
│   │   ├── metrics_http_server.h
│   │   └── websocket_server.h
│   ├── stats/
│   │   ├── leaderboard_store.h
│   │   ├── match_stats.h
│   │   └── player_profile_service.h
│   └── storage/
│       └── postgres_storage.h
├── src/
│   ├── CMakeLists.txt
│   ├── main.cpp
│   ├── core/
│   │   ├── config.cpp
│   │   └── game_loop.cpp
│   ├── game/
│   │   ├── combat.cpp
│   │   ├── game_session.cpp
│   │   └── projectile.cpp
│   ├── matchmaking/
│   │   ├── match.cpp
│   │   ├── match_notification_channel.cpp
│   │   ├── match_queue.cpp
│   │   ├── match_request.cpp
│   │   └── matchmaker.cpp
│   ├── network/
│   │   ├── metrics_http_server.cpp
│   │   └── websocket_server.cpp
│   ├── stats/
│   │   ├── leaderboard_store.cpp
│   │   ├── match_stats.cpp
│   │   └── player_profile_service.cpp
│   └── storage/
│       └── postgres_storage.cpp
└── tests/
    ├── CMakeLists.txt
    └── unit/
        ├── test_combat.cpp
        ├── test_config.cpp
        ├── test_game_loop.cpp
        ├── test_game_session.cpp
        ├── test_leaderboard_store.cpp
        ├── test_match_queue.cpp
        ├── test_match_stats.cpp
        ├── test_matchmaker.cpp
        ├── test_player_profile_service.cpp
        └── test_projectile.cpp
```

## 주요 기능
- 매치 후 통계 수집 (샷, 히트, 정확도, 데미지, 킬, 데스)
- ELO 등급 조정 (K-factor 25)
- 전역 리더보드 (등급별 정렬, Top N 쿼리)
- HTTP API (`GET /profiles/{id}`, `GET /leaderboard`, `GET /metrics`)
- Prometheus 메트릭 노출

## 빌드 가능 여부
✅ 완전 빌드 가능 (코드 포함 스냅샷)

## 이전 스냅샷과의 차이
- 006_matchmaking 기반 + stats/ 모듈 추가
- network/metrics_http_server 추가
- 테스트: test_leaderboard_store.cpp, test_match_stats.cpp, test_player_profile_service.cpp 추가

## 다음 버전과의 차이
- v1.4.0: UDP 넷코드 추가 (WebSocket → UDP 전환)

## 빌드 방법
```bash
cd snapshots/007_stats/server
mkdir build && cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=$VCPKG_ROOT/scripts/buildsystems/vcpkg.cmake
make -j$(nproc)
```

## 테스트 실행
```bash
cd build
ctest --output-on-failure
```
