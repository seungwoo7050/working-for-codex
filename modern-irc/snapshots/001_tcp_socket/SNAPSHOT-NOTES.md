# Snapshot 001: TCP Socket Server

## 패치 정보
- **패치 ID**: 1.0.1
- **패치 이름**: TCP 소켓 서버
- **대응 커밋**: `feat(network): implement TCP socket server with poll()`

## 구현 내용
이 스냅샷은 기본 TCP 소켓 서버를 구현합니다.

### 포함된 기능
- `Socket` 클래스 구현
  - `create()`: socket() 시스템 호출
  - `bind()`: 포트 바인딩
  - `listen()`: 연결 대기
  - `accept()`: 클라이언트 연결 수락
  - `setNonBlocking()`: 논블로킹 모드 설정
  - `setReuseAddr()`: SO_REUSEADDR 옵션 설정

### 포함된 파일
```
src/
├── main.cpp
└── network/
    ├── Socket.cpp
    └── Socket.hpp
```

## 빌드 및 테스트
```bash
make
./ircserv 6667 password

# 다른 터미널에서 테스트
nc localhost 6667
```

## 제한 사항
- poll() 이벤트 루프 없음 (다음 스냅샷에서 구현)
- 클라이언트 연결 즉시 종료 (Client 클래스 없음)
- 메시지 처리 없음

## 다음 단계
- **Snapshot 002**: poll() 기반 이벤트 루프 추가
