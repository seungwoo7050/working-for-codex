# 스냅샷 008_udp_netcode (v1.4.0)

> UDP 넷코드 코어 완료 시점 (코드 기반 스냅샷)

## 대응 버전
- **버전**: v1.4.0
- **패치 범위**: 1.4.0-p1 ~ 1.4.0-p3

## 패치 내용
| 패치 ID | 작업 | 핵심 파일 |
|---------|------|----------|
| 1.4.0-p1 | UDP 소켓 래퍼 구현 | `network/udp_socket.*` |
| 1.4.0-p2 | UDP 게임 서버 코어 | `network/udp_game_server.*`, `network/packet_types.*` |
| 1.4.0-p3 | 스냅샷/델타 동기화 | `network/snapshot_manager.*` |

## 이전 스냅샷과의 차이
- 007_stats 기반 + UDP 네트워크 모듈 추가
- 신규 파일: udp_socket.h/cpp, packet_types.h/cpp, udp_game_server.h/cpp, snapshot_manager.h/cpp
- 테스트: test_packet_types.cpp, test_snapshot_manager.cpp 추가

## 주요 기능
- Boost.Asio 기반 UDP 소켓 추상화
- 비동기 패킷 송수신
- 권위 서버 모델 구현
- 패킷 프로토콜 정의 (CONNECT, INPUT, STATE 등)
- 스냅샷 생성 및 저장
- 델타 압축 (변경분만 전송)
- 스냅샷 보간 (지연 보상용)

## 포함된 모듈 (신규)
```
server/
├── include/pvpserver/
│   └── network/
│       ├── udp_socket.h          # UDP 소켓 래퍼
│       ├── packet_types.h        # 패킷 타입 정의
│       ├── udp_game_server.h     # UDP 게임 서버
│       └── snapshot_manager.h    # 스냅샷/델타 관리
├── src/
│   └── network/
│       ├── udp_socket.cpp
│       ├── packet_types.cpp
│       ├── udp_game_server.cpp
│       └── snapshot_manager.cpp
└── tests/
    └── unit/
        ├── test_packet_types.cpp
        └── test_snapshot_manager.cpp
```

## 아키텍처 변경
```
v1.3.0:
Client ──WebSocket──> WebSocketServer ──> GameSession

v1.4.0:
Client ──UDP──> UdpGameServer ──> GameSession
                    │
                    └── SnapshotManager (상태 저장/델타 계산)
```

## 빌드 가능 여부
✅ 완전 빌드 가능 (WebSocket과 UDP 공존)

## 다음 버전과의 차이
- v1.4.1: 클라이언트 예측, 리컨실리에이션, 입력 버퍼링, 지연 보상 추가

## 빌드 방법
```bash
cd server
mkdir build && cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=$VCPKG_ROOT/scripts/buildsystems/vcpkg.cmake
make -j$(nproc)
```

## 테스트 실행
```bash
cd build
ctest --output-on-failure -L unit
```

## 주요 인터페이스

### UdpSocket
```cpp
UdpSocket socket(io_context, 7777);
socket.StartReceive([](const std::vector<uint8_t>& data, const Endpoint& sender) {
    // 패킷 처리
});
socket.SendTo(data, target);
socket.Broadcast(data);
```

### SnapshotManager
```cpp
SnapshotManager manager;
auto snapshot = manager.CreateSnapshot(players, projectiles);
manager.SaveSnapshot(snapshot);
auto delta = manager.CalculateDelta(baseSeq, targetSeq);
```

## 성능 특성
| 메트릭 | 측정값 |
|--------|--------|
| 스냅샷 직렬화 | < 0.1ms |
| 델타 계산 | < 0.2ms |
| 패킷 크기 (2 플레이어) | ~100 bytes |
