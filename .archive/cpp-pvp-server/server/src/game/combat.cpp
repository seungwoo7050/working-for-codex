// [FILE]
// - 목적: 전투 시스템 핵심 컴포넌트 (HP, 데미지, 전투 로그)
// - 주요 역할: 체력 관리, 데미지 적용, 전투 이벤트 기록
// - 관련 클론 가이드 단계: [CG-v1.1.0] 전투 시스템
// - 권장 읽는 순서: HealthComponent → CombatLog
//
// [LEARN] 게임에서 HP 시스템은 단순해 보이지만,
//         "0 이하 처리", "사망 상태 체크", "리스폰" 등 엣지 케이스가 많다.

#include "pvpserver/game/combat.h"

#include <algorithm>

namespace pvpserver {

// [Order 1] HealthComponent - 플레이어 체력 관리
// - max_hp: 최대 체력 (기본 100)
// - current_: 현재 체력
// [LEARN] C++에서 생성자 초기화 리스트(: member(value))는
//         C의 struct 초기화보다 효율적. 복사 대신 직접 초기화.
HealthComponent::HealthComponent(int max_hp) : max_(max_hp), current_(max_hp) {}

int HealthComponent::current() const noexcept { return current_; }

int HealthComponent::max() const noexcept { return max_; }

// 생존 여부 확인
bool HealthComponent::is_alive() const noexcept { return current_ > 0; }

// [Order 2] ApplyDamage - 데미지 적용
// - 반환값: true = 이 데미지로 사망, false = 아직 생존
// [LEARN] 이미 죽은 상태에서 데미지 무시 (중복 사망 방지)
bool HealthComponent::ApplyDamage(int amount) {
    if (amount <= 0) {
        return false;  // 0 이하 데미지 무시
    }
    if (!is_alive()) {
        return false;  // 이미 죽은 상태
    }
    // std::max로 HP가 음수가 되지 않도록 보장
    current_ = std::max(0, current_ - amount);
    return current_ == 0;  // 이 데미지로 죽었는지 반환
}

// 체력 완전 회복 (리스폰 시 호출)
void HealthComponent::Reset() { current_ = max_; }

// [Order 3] CombatLog - 전투 이벤트 링 버퍼
// - 최근 N개의 전투 이벤트만 저장 (메모리 제한)
// - 디버깅, 리플레이, 통계 분석용
// [LEARN] 링 버퍼(circular buffer)는 고정 크기 배열에서
//         가장 오래된 데이터를 덮어쓰는 자료구조.
//         여기서는 deque + pop_front로 간단히 구현.
CombatLog::CombatLog(std::size_t capacity) : capacity_(capacity) {}

void CombatLog::Add(const CombatEvent& event) {
    if (capacity_ == 0) {
        return;
    }
    events_.push_back(event);
    if (events_.size() > capacity_) {
        events_.pop_front();  // 가장 오래된 이벤트 제거
    }
}

// 현재 저장된 모든 이벤트의 스냅샷 반환
std::vector<CombatEvent> CombatLog::Snapshot() const {
    return std::vector<CombatEvent>(events_.begin(), events_.end());
}

std::size_t CombatLog::Size() const noexcept { return events_.size(); }

std::size_t CombatLog::Capacity() const noexcept { return capacity_; }

}  // namespace pvpserver
