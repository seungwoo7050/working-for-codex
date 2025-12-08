# Snapshot 012: Anti-Cheat System (코드 기반 스냅샷)

**버전**: v2.1.0-v2.1.4
**날짜**: 2024
**이전 스냅샷**: 011_distributed

## 이전 스냅샷과의 차이
- 011_distributed 기반 + anticheat/ 모듈 추가
- 신규 파일: hit_validator.h/cpp, movement_validator.h/cpp, anomaly_detector.h/cpp, replay_recorder.h/cpp, ban_service.h/cpp

## 주요 변경 사항

### 1. 히트 검증 시스템 (v2.1.0)
- **HitValidator**: 서버 측 히트 검증 with 지연 보상
- **RaycastSystem**: AABB/캡슐 충돌 감지
- **WorldStateBuffer**: 과거 월드 상태 버퍼 (최대 200ms 되감기)
- 히트박스 기반 데미지 계산 (HEAD: 2.5x, BODY: 1.0x, LIMB: 0.75x)

### 2. 이동 검증기 (v2.1.1)
- **MovementValidator**: 속도핵, 텔레포트, 월핵 탐지
- 플레이어 상태별 속도 제한 (스프린트, 슬로우)
- 장애물 레이캐스트로 벽 통과 감지
- 위반 카운터 추적

### 3. 이상 탐지 시스템 (v2.1.2)
- **CombatStats**: 전투 통계 (정확도, 헤드샷, 킬스트릭, 반응시간)
- **AnomalyDetector**: Z-Score 기반 이상치 분석
- **SuspicionSystem**: 의심 레벨 관리 (NONE → LOW → MEDIUM → HIGH → CRITICAL)
- 글로벌 통계 기준 비교

### 4. 리플레이 시스템 (v2.1.3)
- **ReplayRecorder**: 게임 프레임 녹화
- **ReplayStorage**: 리플레이 저장/로드 인터페이스
- **InMemoryReplayStorage**: 인메모리 구현 (테스트용)
- 플레이어 시점(POV) 추출 지원

### 5. 밴 시스템 (v2.1.4)
- **BanService**: 플레이어/HWID/IP 밴 관리
- **BanPolicy**: 위반 타입별 밴 기간 정책
- **HardwareIdValidator**: HWID 검증 및 VM 탐지
- 밴 이력 관리 및 자동 만료

## 파일 구조

```
server/
├── include/pvpserver/anticheat/
│   ├── hit_validator.h       # 히트 검증, Vec3, AABB, 레이캐스트
│   ├── movement_validator.h  # 이동 검증
│   ├── anomaly_detector.h    # 이상 탐지, 의심 시스템
│   ├── replay_recorder.h     # 리플레이 녹화/저장
│   └── ban_service.h         # 밴 서비스
├── src/anticheat/
│   ├── hit_validator.cpp
│   ├── movement_validator.cpp
│   ├── anomaly_detector.cpp
│   ├── replay_recorder.cpp
│   └── ban_service.cpp
└── tests/unit/
    ├── test_hit_validator.cpp
    ├── test_movement_validator.cpp
    ├── test_anomaly_detector.cpp
    └── test_ban_service.cpp
```

## 의존성

기존 의존성 유지:
- Boost.Asio/Beast
- GTest (테스트)

## 주요 인터페이스

### HitValidator
```cpp
HitResult ValidateHit(const HitRequest& request);
void RecordWorldState(int64_t tick, const WorldState& state);
```

### MovementValidator
```cpp
MovementCheck ValidateMovement(const std::string& player_id,
                               const Vec3& old_pos, const Vec3& new_pos,
                               float delta_time);
```

### AnomalyDetector
```cpp
AnomalyScore Analyze(const CombatStats& stats);
```

### SuspicionSystem
```cpp
void RecordViolation(const std::string& player_id, const Violation& v);
SuspicionLevel GetLevel(const std::string& player_id) const;
```

### BanService
```cpp
BanCheckResult CheckConnection(const std::string& player_id,
                               const std::string& hardware_id,
                               const std::string& ip_address);
void BanPlayer(const BanRecord& record);
```

## 탐지 임계값

| 항목 | 기본값 |
|------|--------|
| 이동 속도 허용 오차 | 10% |
| 텔레포트 임계값 | 최대속도 × 3 |
| 최대 되감기 시간 | 200ms |
| Z-Score 임계값 | 3.0σ |
| 의심 레벨 CRITICAL | 0.9+ |

## 밴 정책 (기본)

| 위반 타입 | 1차 | 2차 | 3차+ |
|----------|-----|-----|------|
| 속도핵 | 7일 | 30일 | 영구 |
| 에임봇 | 30일 | 영구 | 영구 |
| 월핵 | 7일 | 30일 | 영구 |
| 텔레포트 | 24시간 | 7일 | 30일 |
| 이상 통계 | 경고 | 24시간 | 7일 |

## 테스트 커버리지

- 히트 검증: Vec3 연산, AABB 충돌, 레이캐스트, 데미지 계산
- 이동 검증: 속도핵, 텔레포트, 월핵 탐지
- 이상 탐지: CombatStats, Z-Score 분석, 의심 레벨
- 밴 서비스: 밴 적용/해제, HWID/IP 밴, 정책 관리

## 다음 단계

- Redis 기반 ReplayStorage 구현
- 분산 밴 시스템 (클러스터 간 동기화)
- 실시간 관리자 알림
- 머신러닝 기반 치터 탐지
