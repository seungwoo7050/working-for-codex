# 스냅샷 009_prediction (v1.4.1)

> 클라이언트 예측 & 리컨실리에이션 완료 시점 (코드 기반 스냅샷)

## 대응 버전
- **버전**: v1.4.1
- **패치 범위**: 1.4.1-p1 ~ 1.4.1-p4

## 패치 내용
| 패치 ID | 작업 | 핵심 파일 |
|---------|------|----------|
| 1.4.1-p1 | 클라이언트 예측 엔진 | `netcode/client_prediction.*` |
| 1.4.1-p2 | 리컨실리에이션 시스템 | `netcode/reconciliation.*` |
| 1.4.1-p3 | 입력 버퍼링 | `netcode/input_buffer.*` |
| 1.4.1-p4 | 지연 보상 | `netcode/lag_compensation.*` |

## 이전 스냅샷과의 차이
- 008_udp_netcode 기반 + netcode/ 모듈 추가
- 신규 파일: client_prediction.h/cpp, reconciliation.h/cpp, input_buffer.h/cpp, lag_compensation.h/cpp

## 주요 기능
- 클라이언트 측 입력 예측 (즉각적인 반응)
- 서버 상태와 불일치 시 리컨실리에이션
- 부드러운 보정 (스냅 방지)
- 서버 측 입력 버퍼링 (지터 완화)
- 지연 보상된 히트 판정

## 포함된 모듈 (신규)
```
server/
├── include/pvpserver/
│   └── netcode/                    # 신규 모듈
│       ├── client_prediction.h
│       ├── reconciliation.h
│       ├── input_buffer.h
│       └── lag_compensation.h
└── src/
    └── netcode/
        ├── client_prediction.cpp
        ├── reconciliation.cpp
        ├── input_buffer.cpp
        └── lag_compensation.cpp
```

## 핵심 알고리즘

### 클라이언트 예측
```
1. 입력 발생
2. 로컬에서 즉시 시뮬레이션
3. 서버로 입력 전송
4. 예측 상태 저장 (리컨실리에이션용)
5. 서버 확인 시 예측 정리
```

### 리컨실리에이션
```
1. 서버 상태 수신
2. 해당 시퀀스의 예측과 비교
3. 오차 > 임계값 시:
   - 서버 상태 기준 설정
   - 미확인 입력 재적용
4. 부드러운 보정 적용
```

### 지연 보상
```
1. 히트 요청 수신 (클라이언트 타임스탬프 포함)
2. 과거 월드 상태 조회
3. 과거 상태에서 레이캐스트
4. 결과를 현재에 적용
```

## 빌드 가능 여부
✅ 완전 빌드 가능

## 성능 특성
| 메트릭 | 측정값 |
|--------|--------|
| 예측 계산 | < 0.1ms |
| 리컨실리에이션 (10 입력) | < 0.5ms |
| 지연 보상 조회 | < 0.2ms |
| 입력 버퍼 지연 | 50ms (설정 가능) |
