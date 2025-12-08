# 스냅샷 006: 매치메이킹 (v1.2.0)

## 패치 범위
- 1.2.1: MatchmakingService
- 1.2.2: Redis 큐 연동 (InMemoryMatchQueue로 구현)
- 1.2.3: ELO 계산
- 1.2.4: 매치 생성 로직
- 1.2.5: 결과 기록

## 포함 기능 (v1.0.0 + v1.1.0 기능 포함)
- **WebSocket 서버**: Boost.Beast 기반 비동기 WebSocket 서버
- **60 TPS 게임 루프**: 고정 시간 단계 스케줄러
- **전투 시스템**: 발사체, 충돌 검출, 데미지
- **매치메이킹**: ELO 기반 플레이어 매칭
  - 동적 허용 오차 확장 (5초마다 ±25)
  - InMemoryMatchQueue (Redis 스텁 포함)
  - MatchNotificationChannel

## 미포함 기능 (v1.3.0에서 추가)
- 통계/랭킹 (PlayerProfileService, LeaderboardStore)
- HTTP 메트릭/프로필 API

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
│   │   └── websocket_server.h
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
│   │   └── websocket_server.cpp
│   └── storage/
│       └── postgres_storage.cpp
└── tests/
    ├── CMakeLists.txt
    └── unit/
        ├── test_combat.cpp
        ├── test_config.cpp
        ├── test_game_loop.cpp
        ├── test_game_session.cpp
        ├── test_match_queue.cpp
        ├── test_matchmaker.cpp
        └── test_projectile.cpp
```

## 빌드 방법
```bash
cd snapshots/006_matchmaking/server
mkdir build && cd build
cmake .. -DCMKG_TOOLCHAIN_FILE=$VCPKG_ROOT/scripts/buildsystems/vcpkg.cmake
cmake --build .
./pvpserver
```

## 테스트 실행
```bash
cd build
ctest --output-on-failure
```

## 완료 기준
- [x] 대기열 참가 → 매칭 완료
- [x] 비슷한 ELO끼리 매칭
- [x] 매치 이벤트 발행

## 빌드 가능 여부
✅ 빌드 가능 (코드 포함 스냅샷)

## 이전 스냅샷과의 차이
- 005_combat 기반 + matchmaking/ 모듈 추가
- 테스트: test_matchmaker.cpp, test_match_queue.cpp 추가
