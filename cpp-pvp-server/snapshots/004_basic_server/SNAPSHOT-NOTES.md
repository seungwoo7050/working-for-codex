# 스냅샷 004: 기본 게임 서버 (v1.0.0)

## 패치 범위
- 1.0.1: WebSocket 서버 구현
- 1.0.2: 연결 관리자
- 1.0.3: GameLoop (60 TPS)
- 1.0.4: GameSession 관리
- 1.0.5: 플레이어 이동 처리
- 1.0.6: PostgreSQL 연동

## 포함 기능
- **WebSocket 서버**: Boost.Beast 기반 비동기 WebSocket 서버
- **60 TPS 게임 루프**: 고정 시간 단계 스케줄러
- **플레이어 상태 관리**: 위치, 방향 추적
- **WASD 이동**: 정규화된 이동 물리
- **PostgreSQL 연동**: 세션 이벤트 기록

## 미포함 기능 (v1.1.0+에서 추가)
- 전투 시스템 (Projectile, Combat, HealthComponent)
- 매치메이킹
- 통계/랭킹
- HTTP 메트릭 서버

## 빌드 방법
```bash
cd server
mkdir build && cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=$VCPKG_ROOT/scripts/buildsystems/vcpkg.cmake
cmake --build .
./pvpserver
```

## 테스트 실행
```bash
cd build
ctest --output-on-failure
```

## 완료 기준
- [x] 2명 동시 접속 가능
- [x] 60 TPS 고정 틱레이트 유지
- [x] WASD 이동 동기화
- [x] 플레이어 데이터 저장/로드

## 빌드 가능 여부
✅ 빌드 가능

## 구조적 참고사항
- `MovementInput`에서 `fire` 필드 제거됨
- `GameSession`에서 전투 관련 코드 제거됨
- `WebSocketServer`에서 death 이벤트 브로드캐스트 제거됨
