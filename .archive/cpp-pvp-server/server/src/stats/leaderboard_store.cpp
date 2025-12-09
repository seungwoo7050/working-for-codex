// [FILE]
// - 목적: 리더보드 저장소 (순위표 관리)
// - 주요 역할: 플레이어 점수 저장/조회, 상위 N명 조회
// - 관련 클론 가이드 단계: [v1.3.0] 통계 시스템
// - 권장 읽는 순서: Upsert → TopN → InMemory vs Redis 구현
//
// [LEARN] 리더보드 자료구조:
//         - InMemory: std::map (점수→플레이어 집합) + unordered_map (플레이어→점수)
//         - Redis: Sorted Set (ZADD, ZREVRANGE) - 대규모 분산 환경용
//
// 두 가지 구현을 제공하여 환경에 따라 선택:
//         - 단일 서버: InMemoryLeaderboardStore
//         - 분산 서버: RedisLeaderboardStore

#include "pvpserver/stats/leaderboard_store.h"

#include <algorithm>
#include <iostream>

namespace pvpserver {

// [Order 1] RemoveFromOrdered - 정렬된 맵에서 플레이어 제거
// [LEARN] 점수 변경 시 기존 점수에서 먼저 제거해야 함
void InMemoryLeaderboardStore::RemoveFromOrdered(const std::string& player_id, int score) {
    auto ordered_it = ordered_.find(score);
    if (ordered_it == ordered_.end()) {
        return;
    }
    ordered_it->second.erase(player_id);
    // 해당 점수에 플레이어가 없으면 점수 엔트리도 삭제
    if (ordered_it->second.empty()) {
        ordered_.erase(ordered_it);
    }
}

// [Order 2] Upsert - 점수 삽입/갱신
// [LEARN] upsert = update + insert. 있으면 갱신, 없으면 삽입.
//         두 자료구조 동기화 필요 (scores_, ordered_)
void InMemoryLeaderboardStore::Upsert(const std::string& player_id, int score) {
    const auto existing = scores_.find(player_id);
    if (existing != scores_.end()) {
        if (existing->second == score) {
            return;  // 점수 변화 없음
        }
        // 기존 점수에서 제거
        RemoveFromOrdered(player_id, existing->second);
    }
    scores_[player_id] = score;
    ordered_[score].insert(player_id);  // 새 점수에 추가
}

void InMemoryLeaderboardStore::Erase(const std::string& player_id) {
    const auto existing = scores_.find(player_id);
    if (existing == scores_.end()) {
        return;
    }
    RemoveFromOrdered(player_id, existing->second);
    scores_.erase(existing);
}

// [Order 3] TopN - 상위 N명 조회
// [LEARN] std::map은 키(점수) 기준 정렬됨.
//         역순으로 순회하면 높은 점수부터 조회.
//         C++17 structured binding: auto& [score, players]
std::vector<std::pair<std::string, int>> InMemoryLeaderboardStore::TopN(std::size_t limit) const {
    std::vector<std::pair<std::string, int>> result;
    result.reserve(std::min(limit, scores_.size()));
    std::size_t remaining = limit;
    
    // 역순 반복 (높은 점수부터)
    for (const auto& [score, players] : ordered_) {
        for (const auto& player : players) {
            if (remaining == 0) {
                return result;
            }
            result.emplace_back(player, score);
            --remaining;
        }
        if (remaining == 0) {
            break;
        }
    }
    return result;
}

std::optional<int> InMemoryLeaderboardStore::Get(const std::string& player_id) const {
    const auto it = scores_.find(player_id);
    if (it == scores_.end()) {
        return std::nullopt;
    }
    return it->second;
}

std::size_t InMemoryLeaderboardStore::Size() const { return scores_.size(); }

// ========================================================================
// [Redis 구현] - 분산 환경용 (스텁)
// ========================================================================
// [LEARN] Redis Sorted Set 명령어:
//         - ZADD: 점수와 함께 멤버 추가
//         - ZREVRANGE: 역순으로 범위 조회 (높은 점수부터)
//         - ZSCORE: 특정 멤버 점수 조회
//         - ZREM: 멤버 제거

void RedisLeaderboardStore::Upsert(const std::string& player_id, int score) {
    // ZADD leaderboard score player_id
    std::cout << "redis zadd leaderboard " << score << ' ' << player_id << std::endl;
}

void RedisLeaderboardStore::Erase(const std::string& player_id) {
    // ZREM leaderboard player_id
    std::cout << "redis zrem leaderboard " << player_id << std::endl;
}

std::vector<std::pair<std::string, int>> RedisLeaderboardStore::TopN(std::size_t limit) const {
    // ZREVRANGE leaderboard 0 (limit-1) WITHSCORES
    std::cout << "redis zrevrange leaderboard 0 " << (limit ? limit - 1 : 0) << " withscores"
              << std::endl;
    return {};  // 스텁: 실제 구현 시 파싱 필요
}

std::optional<int> RedisLeaderboardStore::Get(const std::string& player_id) const {
    // ZSCORE leaderboard player_id
    std::cout << "redis zscore leaderboard " << player_id << std::endl;
    return std::nullopt;  // 스텁
}

std::size_t RedisLeaderboardStore::Size() const { return 0; }

// ========================================================================
// [Reader Notes] 리더보드 구현
// ========================================================================
// 1. InMemory 자료구조
//    - scores_: player_id → score (O(1) 조회)
//    - ordered_: score → set<player_id> (정렬된 순회)
//    - 두 자료구조 동기화 필수!
//
// 2. Redis Sorted Set
//    - 내부적으로 Skip List + Hash Table
//    - O(log N) 삽입, O(log N + M) 범위 조회
//    - 분산 환경에서 여러 서버가 공유
//
// 3. 성능 고려
//    - 단일 서버: InMemory 충분 (수십만 플레이어)
//    - 분산 서버: Redis 필수 (상태 공유)
//    - 캐싱: TopN 결과를 잠시 캐싱하여 DB 부하 감소
//
// [v1.3.0] 통계 - 리더보드/순위표 시스템
// ========================================================================

}  // namespace pvpserver
