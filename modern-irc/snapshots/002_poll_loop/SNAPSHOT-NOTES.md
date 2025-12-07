# Snapshot 002: poll() Event Loop

## 패치 정보
- **패치 ID**: 1.0.2
- **패치 이름**: poll() 이벤트 루프
- **대응 커밋**: `feat(network): implement TCP socket server with poll()`

## 구현 내용
이 스냅샷은 poll() 기반 이벤트 멀티플렉싱을 추가합니다.

### 포함된 기능
- `Socket` 클래스 (이전 스냅샷과 동일)
- `Poller` 클래스 구현
  - `addFd()`: 파일 디스크립터 추가
  - `removeFd()`: 파일 디스크립터 제거
  - `poll()`: 이벤트 대기
  - `hasEvent()`: 이벤트 발생 확인

### 포함된 파일
```
src/
├── main.cpp
└── network/
    ├── Socket.cpp
    ├── Socket.hpp
    ├── Poller.cpp
    └── Poller.hpp
```

## 빌드 및 테스트
```bash
make
./ircserv 6667 password

# 테스트 (에코 서버로 동작)
nc localhost 6667
Hello  # 입력하면 에코됨
```

## 빌드 가능 여부
✅ 빌드 가능, 에코 서버로 동작

## 다음 단계
- **Snapshot 003**: Client 클래스 추가
