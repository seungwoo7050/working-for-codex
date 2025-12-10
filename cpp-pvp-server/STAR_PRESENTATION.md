# C++ PVP Server — STAR 방식 프로젝트 발표

> 행동 기반 역량 면접(BEI) 대응을 위한 STAR 구조 발표 자료

---

## 🎯 Situation (상황)

### 프로젝트 배경

**목표 회사**: Nexon, Krafton, Netmarble, Kakao Games

**시장 요구사항**:
- 한국 게임 업계 게임 서버 개발자 채용에서 요구하는 핵심 역량:
  - 실시간 네트워크 프로그래밍
  - 고성능 C++ 개발
  - 동시성 처리 및 상태 동기화
  - 대규모 동시 접속 처리
- 단순 웹 백엔드와 게임 서버는 완전히 다른 도메인
- 실제 플레이 가능한 게임이 있어야 역량 증명

**기술적 도전**:
1. **60 TPS 고정**: 결정론적 시뮬레이션 유지
2. **저지연 통신**: 50ms 이하 응답 시간
3. **치트 방지**: 서버 권위 모델 구현
4. **확장성**: 분산 시스템 패턴 적용

### 개인 역할

게임 서버 개발자로서 전체 아키텍처 설계 및 구현 (싱글 프로젝트)

---

## 📋 Task (과업)

### 핵심 목표

| 영역 | 목표 | 측정 지표 |
|------|------|----------|
| **성능** | 60 TPS 유지 | 틱 편차 ≤ 1ms |
| **지연** | 저지연 통신 | p99 ≤ 50ms |
| **확장성** | 10+ 동시 매치 | 부하 테스트 |
| **공정성** | 치트 방지 | 서버 권위 검증 |

### 체크포인트별 요구사항

| 체크포인트 | 목표 | 증명 사항 |
|-----------|------|----------|
| **A (v1.0-1.3)** | 1v1 듀얼 게임 | 60 TPS, 전투, 매치메이킹, 통계 |
| **v1.4.x** | UDP 넷코드 | 예측, 지연 보상 |
| **v2.x** | 분산 시스템 | 세션 분리, 로드 밸런서, 안티치트 |

---

## 🔧 Action (행동)

### 1단계: 기본 게임 서버 (v1.0.0)

#### 60 TPS 게임 루프 설계
```cpp
class GameLoop {
    std::chrono::nanoseconds target_delta_{16'666'667};  // 60 TPS
    
    void Run() {
        auto next_frame = std::chrono::steady_clock::now();
        
        while (running_) {
            auto frame_start = next_frame;
            
            // 게임 로직 실행
            callback_(TickInfo{tick_++, target_delta_.count(), frame_start});
            
            // 다음 프레임까지 정확한 대기
            next_frame += target_delta_;
            std::this_thread::sleep_until(next_frame);
        }
    }
};
```

**핵심 결정**: `next_frame` 누적 방식
- 이유: 단순 sleep은 드리프트 발생
- 결과: 틱 편차 0.04ms 달성 (목표 1ms)

#### WebSocket 서버 (Boost.Beast)
```cpp
class WebSocketServer {
    void BroadcastState(uint64_t tick) {
        auto states = session_.Snapshot();
        
        for (const auto& state : states) {
            std::ostringstream oss;
            oss << "state " << state.player_id << " "
                << state.x << " " << state.y << " "
                << state.facing_radians << " " << tick;
            
            client->EnqueueMessage(oss.str());
        }
    }
};
```

**성과**: 
- p99 지연 18.3ms (목표 50ms)
- 비동기 I/O로 블로킹 없음

### 2단계: 전투 시스템 (v1.1.0)

#### 발사체 시뮬레이션
```cpp
class Projectile {
    static constexpr double kSpeed = 30.0;     // m/s
    static constexpr double kLifetime = 1.5;   // seconds
    static constexpr double kRadius = 0.2;     // meters
    
    void Advance(double delta_seconds) {
        double distance = kSpeed * delta_seconds;
        x_ += dir_x_ * distance;
        y_ += dir_y_ * distance;
    }
};
```

#### 원-원 충돌 감지
```cpp
void DetectCollisions() {
    const double radius_sum = Projectile::kRadius + kPlayerRadius;
    const double radius_sum_sq = radius_sum * radius_sum;
    
    for (auto& proj : projectiles_) {
        for (auto& [player_id, runtime] : players_) {
            if (proj.OwnerId() == player_id) continue;  // 자기 맞추기 방지
            
            double dx = proj.X() - runtime.state.x;
            double dy = proj.Y() - runtime.state.y;
            double dist_sq = dx * dx + dy * dy;
            
            if (dist_sq <= radius_sum_sq) {
                ApplyDamage(runtime, 20);
                proj.Deactivate();
            }
        }
    }
}
```

**성과**: 전투 틱 처리 0.31ms (목표 0.5ms)

### 3단계: 매치메이킹 (v1.2.0)

#### ELO 버킷팅
```cpp
class InMemoryMatchQueue {
    std::map<int, std::list<QueuedPlayer>> buckets_;  // ELO -> 플레이어
    std::unordered_map<std::string, ...> index_;       // 빠른 조회
};
```

**복잡도 개선**:
- 단순 리스트: O(N²) 매칭
- ELO 버킷팅: O(N log N)

#### 동적 허용 오차
```cpp
int CurrentTolerance(time_point now) const {
    constexpr int kBaseTolerance = 100;
    constexpr int kToleranceStep = 25;
    constexpr double kStepSeconds = 5.0;
    
    double waited = duration<double>(now - enqueued_at_).count();
    int increments = static_cast<int>(std::floor(waited / kStepSeconds));
    
    return kBaseTolerance + increments * kToleranceStep;
}
```

**성과**: 200명 큐에서 2ms 이내 매칭

### 4단계: UDP 넷코드 (v1.4.x)

#### 스냅샷/델타 동기화
```cpp
struct Delta {
    uint32_t baseSequence;
    uint32_t targetSequence;
    std::vector<uint8_t> changes;  // 변경분만
};

// 압축 효과
// 전체 스냅샷: ~500 bytes
// 델타: ~100 bytes (80% 감소)
```

#### 클라이언트 예측 + 리컨실리에이션
```cpp
class Reconciliation {
    ReconciliationResult reconcile(
        const PlayerState& serverState,
        uint32_t serverSequence,
        ClientPrediction& prediction) {
        
        // 1. 서버 상태와 예측 비교
        auto predicted = prediction.getLatestPrediction();
        float error = Distance(serverState, predicted.state);
        
        if (error > THRESHOLD) {
            // 2. 미확인 입력 재시뮬레이션
            auto inputs = prediction.getUnacknowledgedInputs(serverSequence);
            return resimulate(serverState, inputs);
        }
        
        return {false, 0, 0, predicted.state};
    }
};
```

#### 지연 보상
```cpp
class LagCompensation {
    WorldState getWorldStateAt(uint64_t timestamp) const {
        // 과거 상태 조회 (최대 200ms)
        for (auto& entry : history_) {
            if (entry.timestamp <= timestamp) {
                return interpolate(entry, next);
            }
        }
    }
};
```

### 5단계: 분산 시스템 (v2.0.x)

#### Consistent Hash 로드 밸런서
```cpp
class ConsistentHashRing {
    int virtual_nodes_ = 150;  // 균등 분배용
    std::map<size_t, std::string> ring_;
    
    std::string GetNode(const std::string& key) const {
        size_t hash = MurmurHash3(key);
        auto it = ring_.lower_bound(hash);
        if (it == ring_.end()) it = ring_.begin();
        return it->second;
    }
};
```

**성과**: 노드 추가/제거 시 최소 재할당 (< 1/N)

#### Redis 세션 저장소
```cpp
class RedisSessionStore : public SessionStore {
    bool SaveSession(const std::string& session_id, const SessionData& data) {
        // SETEX pvp:session:{id} 3600 {data}
        redis_->setex(key, ttl_seconds_, data.Serialize());
        return true;
    }
};
```

### 6단계: 안티치트 (v2.1.0)

#### 서버 측 히트 검증
```cpp
class HitValidator {
    HitResult ValidateHit(const HitRequest& request) {
        // 1. 지연 보상된 과거 상태 조회
        auto world = state_buffer_.GetStateAt(request.client_timestamp);
        
        // 2. 서버 레이캐스트
        auto hit = raycast_system_.Cast(world, request.origin, request.direction);
        
        if (!hit || hit->entity_id != request.target_id) {
            return {false, "", {}, 0, {}, "Raycast miss"};
        }
        
        return {true, hit->entity_id, hit->hit_point, 20, hit->hitbox, ""};
    }
};
```

#### 이상 탐지 (Z-Score)
```cpp
class AnomalyDetector {
    float CalculateZScore(float value, float mean, float stddev) {
        if (stddev == 0) return 0;
        return (value - mean) / stddev;
    }
    
    AnomalyScore Analyze(const CombatStats& stats) {
        float acc_zscore = CalculateZScore(stats.Accuracy(), 0.25, 0.1);
        float hs_zscore = CalculateZScore(stats.HeadshotRatio(), 0.15, 0.08);
        // Z-Score > 3 = 99.7% 이상치
    }
};
```

---

## 📈 Result (결과)

### 정량적 성과

| 메트릭 | 목표 | 달성 | 상태 |
|--------|------|------|------|
| 서버 틱 레이트 | 60 TPS ±1 | 60 TPS | ✅ |
| 틱 레이트 분산 | ≤ 1.0 ms | 0.04 ms | ✅ |
| WebSocket 지연 | p99 ≤ 50ms | 18.3 ms | ✅ |
| 전투 틱 지속 시간 | < 0.5 ms | 0.31 ms | ✅ |
| 동시 매치 | 10+ | 10+ | ✅ |
| 에러율 | ≤ 0.1% | 0.08% | ✅ |

### 기술적 성취

1. **게임 서버 구현**: 60 TPS 결정론적 시뮬레이션
2. **UDP 넷코드**: 예측, 리컨실리에이션, 지연 보상
3. **분산 시스템**: Consistent Hash, Redis 세션
4. **안티치트**: 히트 검증, 이상 탐지, 리플레이

### 배운 점

1. **결정론적 시뮬레이션**: 고정 시간 단계의 중요성
2. **네트워크 최적화**: 델타 압축, 예측, 보정
3. **치트 방지**: 서버 권위 + 통계적 탐지
4. **C++ 메모리 관리**: RAII, 스마트 포인터

---

## 💡 핵심 기술 역량 증명

### 1. 실시간 시스템
> "60 TPS ±0.04ms 분산으로 결정론적 시뮬레이션"

게임 루프 타이밍의 정확성 확보

### 2. 네트워크 프로그래밍
> "WebSocket + UDP 듀얼 스택, 지연 보상"

실시간 통신의 이론과 실제

### 3. C++ 숙련도
> "RAII, 스마트 포인터, 스레드 안전성"

현대적 C++ 관용구 적용

### 4. 분산 시스템
> "Consistent Hash, Redis 세션, gRPC"

수평 확장 가능한 아키텍처

---

## 🎤 예상 질문 및 답변

### Q1: 60 TPS를 어떻게 정확히 유지하나요?

**답변**: 
- 단순 `sleep(16.67ms)`는 드리프트 발생
- `next_frame` 누적 방식으로 타이밍 보정
- `sleep_until`로 정확한 대기
- 결과: 틱 편차 0.04ms (목표의 4%)

### Q2: 서버 권위 모델의 장단점은?

**답변**:
- **장점**: 치트 완전 방지, 결정론적 시뮬레이션
- **단점**: 서버 부하, 입력 지연
- **보완**: 클라이언트 예측 + 리컨실리에이션

### Q3: 지연 보상이 필요한 이유는?

**답변**:
- 플레이어 A가 t=0에 발사, RTT=100ms
- 서버가 t=50ms에 수신 시 적 B는 다른 위치
- 지연 보상: t=0 시점의 상태로 되돌려 판정
- 결과: 발사자 입장에서 공정한 히트

### Q4: Consistent Hash를 선택한 이유는?

**답변**:
- 노드 추가/제거 시 최소 재할당 (1/N)
- 세션 어피니티 자연스럽게 유지
- 가상 노드로 균등 분배 보장
