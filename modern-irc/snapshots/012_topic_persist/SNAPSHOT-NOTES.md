# Snapshot 012: Topic Persistence (v2.0.0 Complete)

## 패치 정보
- **패치 ID**: 2.0.5
- **패치 이름**: 토픽 영속화
- **버전**: v2.0.0 Advanced Features 완료

## 구현 내용
v2.0.0의 모든 고급 기능이 구현된 상태입니다.

### v1.0에서 추가된 기능 (v2.0)
- **채널 모드**: +i (초대전용), +t (토픽제한), +k (비밀번호), +o (오퍼), +l (인원제한)
- **오퍼레이터 명령**: KICK, INVITE, TOPIC, MODE
- **정보 명령**: WHOIS, WHO
- **RFC 에러 코드**: 21개+ 확장
- **토픽 영속화**: TopicPersistence

### 포함된 파일 (v1.0 대비 추가)
```
src/
├── commands/
│   ├── OperatorCommands.cpp/.hpp   # KICK, INVITE, TOPIC, MODE
│   └── InfoCommands.cpp/.hpp       # WHOIS, WHO
├── core/
│   └── Channel.cpp/.hpp            # 모드 시스템 확장
└── utils/
    └── TopicPersistence.cpp/.hpp   # 토픽 저장/로드
```

## 빌드 가능 여부
✅ 빌드 가능, v2.0 모든 기능 동작

## 미구현 기능 (v3.0에서 구현)
- Config 파일 시스템
- Logger
- Rate Limiting
- OPER, KILL, REHASH, DIE
- STATS, INFO, VERSION, TIME, LUSERS

## 다음 단계
- **Snapshot 013**: Config 파일 시스템 (v3.0 시작)
