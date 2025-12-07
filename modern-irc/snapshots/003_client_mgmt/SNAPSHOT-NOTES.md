# Snapshot 003: Client Management

## 패치 정보
- **패치 ID**: 1.0.3
- **패치 이름**: 클라이언트 연결 관리
- **대응 커밋**: `feat(core): add Client and Channel classes`

## 구현 내용
- `Server` 클래스: 서버 메인 로직
- `Client` 클래스: 클라이언트 상태 관리
  - 수신/송신 버퍼
  - 닉네임, 유저네임, 리얼네임
  - 인증/등록 상태

### 포함된 파일
```
src/
├── main.cpp
├── core/
│   ├── Server.cpp/.hpp
│   └── Client.cpp/.hpp
└── network/
    ├── Socket.cpp/.hpp
    └── Poller.cpp/.hpp
```

## 빌드 가능 여부
✅ 빌드 가능 (에코 서버로 동작)

## 다음 단계
- **Snapshot 004**: Message 파서 추가
