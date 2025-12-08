# 스냅샷 005: 전투 시스템 (v1.1.0)

## 패치 범위
- 1.1.1: Combat 컴포넌트
- 1.1.2: Projectile 시스템
- 1.1.3: Collision 검출
- 1.1.4: Damage 계산
- 1.1.5: HP/사망 처리

## 포함 기능 (v1.0.0 기능 포함)
- **WebSocket 서버**: Boost.Beast 기반 비동기 WebSocket 서버
- **60 TPS 게임 루프**: 고정 시간 단계 스케줄러
- **플레이어 상태 관리**: 위치, 방향, 체력 추적
- **WASD 이동**: 정규화된 이동 물리
- **PostgreSQL 연동**: 세션 이벤트 기록
- **발사체 시스템**: 30 m/s 직선 운동, 1.5초 수명
- **충돌 검출**: 원-원 교차 (플레이어 0.5m, 발사체 0.2m)
- **데미지 시스템**: 히트당 20 HP, 100 HP 시작
- **사망 처리**: HP 0시 사망, death 이벤트 브로드캐스트

## 미포함 기능 (v1.2.0+에서 추가)
- 매치메이킹 (Matchmaker, MatchQueue)
- 통계/랭킹 (PlayerProfileService, LeaderboardStore)
- HTTP 메트릭 서버

## 빌드 방법
```bash
cd server
mkdir build && cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=$VCPKG_ROOT/scripts/buildsystems/vcpkg.cmake
cmake --build .
./pvpserver
```

## 테스트 방법
```bash
cd server/build
ctest --output-on-failure
```

## 완료 기준
- [x] 스페이스바(fire) → 투사체 발사
- [x] 투사체 충돌 검출 정확
- [x] HP 0 → 사망 이벤트 발생

## 빌드 가능 여부
✅ 빌드 가능 (vcpkg 환경 필요)

## 파일 구조

```
server/
├── CMakeLists.txt
├── vcpkg.json
├── include/pvpserver/
│   ├── core/
│   │   ├── config.h
│   │   └── game_loop.h
│   ├── game/
│   │   ├── combat.h        # 신규
│   │   ├── game_session.h  # 업데이트 (combat 지원)
│   │   ├── movement.h      # 업데이트 (fire 필드)
│   │   ├── player_state.h  # 업데이트 (stats 필드)
│   │   └── projectile.h    # 신규
│   ├── network/
│   │   └── websocket_server.h
│   └── storage/
│       └── postgres_storage.h
├── src/
│   ├── CMakeLists.txt      # 업데이트 (combat, projectile 추가)
│   ├── main.cpp
│   ├── core/
│   ├── game/
│   │   ├── combat.cpp      # 신규
│   │   ├── game_session.cpp # 업데이트
│   │   └── projectile.cpp  # 신규
│   ├── network/
│   └── storage/
└── tests/
    ├── CMakeLists.txt
    └── unit/
        ├── test_combat.cpp     # 신규
        ├── test_config.cpp
        ├── test_game_loop.cpp
        ├── test_game_session.cpp
        └── test_projectile.cpp # 신규
```

## 004 → 005 변경점

### 추가된 파일
- `include/pvpserver/game/combat.h`
- `include/pvpserver/game/projectile.h`
- `src/game/combat.cpp`
- `src/game/projectile.cpp`
- `tests/unit/test_combat.cpp`
- `tests/unit/test_projectile.cpp`

### 수정된 파일
- `include/pvpserver/game/movement.h` - `fire` 필드 추가
- `include/pvpserver/game/player_state.h` - `shots_fired`, `hits_landed`, `deaths` 추가
- `include/pvpserver/game/game_session.h` - combat 관련 메서드/멤버 추가
- `src/game/game_session.cpp` - 전투 로직 구현
- `src/CMakeLists.txt` - combat.cpp, projectile.cpp 추가
- `tests/CMakeLists.txt` - test_combat.cpp, test_projectile.cpp 추가
