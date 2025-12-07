# Snapshot 004: Message Parser

## 패치 정보
- **패치 ID**: 1.0.4
- **패치 이름**: IRC 메시지 파서
- **대응 커밋**: `feat(core): implement IRC message parser`

## 구현 내용
- `Message` 클래스: IRC 프로토콜 메시지 파싱
  - prefix, command, params, trailing 파싱
  - IRC 메시지 포맷 생성
- `StringUtils` 클래스: 문자열 유틸리티 함수
  - split, trim, toUpper, toLower
  - 유효성 검사 (nick, channel)
- `Server` 수정: processMessage 메서드 추가

### 포함된 파일
```
src/
├── main.cpp
├── core/
│   ├── Server.cpp/.hpp
│   ├── Client.cpp/.hpp
│   └── Message.cpp/.hpp
├── network/
│   ├── Socket.cpp/.hpp
│   └── Poller.cpp/.hpp
└── utils/
    └── StringUtils.cpp/.hpp
```

## 빌드 방법
```bash
make        # 빌드
./ircserv 6667 password  # 실행
make clean  # 오브젝트 파일 정리
make fclean # 전체 정리
make re     # 재빌드
```

## 테스트
```bash
nc localhost 6667
# 메시지 전송 - 파싱 결과 확인
```

## 빌드 가능 여부
✅ 빌드 가능 (메시지 파싱 기능 추가)

## 다음 단계
- **Snapshot 005**: 인증 명령어 (PASS, NICK, USER, QUIT)
