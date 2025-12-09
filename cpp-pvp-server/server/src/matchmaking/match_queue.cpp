// [FILE]
// - 목적: 매치메이킹 대기열 구현 (인메모리 + Redis 스텁)
// - 주요 역할: 플레이어 대기열 관리, ELO 기반 버킷 정렬, 순서 보장
// - 관련 클론 가이드 단계: [CG-v1.2.0] 매치메이킹
// - 권장 읽는 순서: matchmaker.cpp 이후 이 파일 참고
//
// [LEARN] 매치메이킹 큐 자료구조:
//         - buckets_: ELO별로 플레이어를 그룹화 (map<elo, list>)
//         - index_: player_id로 빠른 조회를 위한 인덱스
//         - 이중 자료구조로 O(1) 조회 + ELO 기반 정렬을 동시에 달성
//
// [Reader Notes]
// - InMemoryMatchQueue: 단일 서버용 인메모리 구현
// - RedisMatchQueue: 분산 서버용 Redis 구현 (현재 스텁)
// - 다음에 읽을 파일: matchmaker.cpp (이 큐를 사용하는 매칭 로직)

#include "pvpserver/matchmaking/match_queue.h"

#include <algorithm>
#include <iostream>
#include <sstream>

namespace pvpserver {

InMemoryMatchQueue::InMemoryMatchQueue() = default;
InMemoryMatchQueue::~InMemoryMatchQueue() = default;

// [Order 1] Upsert - 플레이어를 대기열에 추가/갱신
// [LEARN] Upsert = Update + Insert
//         이미 대기 중인 플레이어면 기존 항목 제거 후 새로 추가.
//         ELO 버킷 내에서 order 순서를 유지하여 "먼저 온 사람 먼저 매칭".
void InMemoryMatchQueue::Upsert(const MatchRequest& request, std::uint64_t order) {
    // 기존 항목이 있으면 제거 (ELO가 변경되었을 수 있음)
    auto existing = index_.find(request.player_id());
    if (existing != index_.end()) {
        auto bucket_it = buckets_.find(existing->second.first);
        if (bucket_it != buckets_.end()) {
            bucket_it->second.erase(existing->second.second);
            if (bucket_it->second.empty()) {
                buckets_.erase(bucket_it);
            }
        }
        index_.erase(existing);
    }

    auto& bucket = buckets_[request.elo()];
    BucketEntry entry{request, order};
    auto insert_pos = bucket.end();
    for (auto it = bucket.begin(); it != bucket.end(); ++it) {
        if (order < it->order) {
            insert_pos = it;
            break;
        }
    }
    auto inserted = bucket.insert(insert_pos, std::move(entry));
    index_[inserted->request.player_id()] = {request.elo(), inserted};
}

bool InMemoryMatchQueue::Remove(const std::string& player_id) {
    auto existing = index_.find(player_id);
    if (existing == index_.end()) {
        return false;
    }
    auto bucket_it = buckets_.find(existing->second.first);
    if (bucket_it == buckets_.end()) {
        index_.erase(existing);
        return false;
    }
    bucket_it->second.erase(existing->second.second);
    if (bucket_it->second.empty()) {
        buckets_.erase(bucket_it);
    }
    index_.erase(existing);
    return true;
}

std::vector<QueuedPlayer> InMemoryMatchQueue::FetchOrdered() const {
    std::vector<QueuedPlayer> ordered;
    ordered.reserve(index_.size());
    for (const auto& bucket : buckets_) {
        for (const auto& entry : bucket.second) {
            ordered.push_back(QueuedPlayer{entry.request, entry.order});
        }
    }
    std::sort(ordered.begin(), ordered.end(), [](const QueuedPlayer& lhs, const QueuedPlayer& rhs) {
        if (lhs.request.elo() == rhs.request.elo()) {
            return lhs.order < rhs.order;
        }
        return lhs.request.elo() < rhs.request.elo();
    });
    return ordered;
}

std::size_t InMemoryMatchQueue::Size() const { return index_.size(); }

std::string InMemoryMatchQueue::Snapshot() const {
    std::ostringstream oss;
    bool first = true;
    for (const auto& bucket : buckets_) {
        for (const auto& entry : bucket.second) {
            if (!first) {
                oss << ",";
            }
            first = false;
            oss << entry.request.player_id() << ':' << entry.request.elo();
        }
    }
    return oss.str();
}

RedisMatchQueue::RedisMatchQueue(std::ostream& stream) : stream_(&stream) {}

void RedisMatchQueue::Upsert(const MatchRequest& request, std::uint64_t order) {
    if (stream_) {
        (*stream_) << "ZADD matchmaking_queue " << request.elo() << ' ' << request.player_id()
                   << std::endl;
    }
    fallback_.Upsert(request, order);
}

bool RedisMatchQueue::Remove(const std::string& player_id) {
    if (stream_) {
        (*stream_) << "ZREM matchmaking_queue " << player_id << std::endl;
    }
    return fallback_.Remove(player_id);
}

std::vector<QueuedPlayer> RedisMatchQueue::FetchOrdered() const {
    if (stream_) {
        (*stream_) << "ZRANGE matchmaking_queue 0 -1 WITHSCORES" << std::endl;
    }
    return fallback_.FetchOrdered();
}

std::size_t RedisMatchQueue::Size() const { return fallback_.Size(); }

std::string RedisMatchQueue::Snapshot() const { return fallback_.Snapshot(); }

}  // namespace pvpserver
