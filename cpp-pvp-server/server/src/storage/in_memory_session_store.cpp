// [FILE]
// - 목적: 인메모리 세션 저장소 (단일 서버용)
// - 주요 역할: 세션 데이터를 메모리에 저장, TTL 기반 자동 만료
// - 관련 클론 가이드 단계: [CG-02.00] 분산 시스템 - 세션 관리
// - 권장 읽는 순서: SaveSession → GetSession → CleanupLoop
//
// [LEARN] 인메모리 vs Redis 세션:
//         - 인메모리: 단일 서버, 빠름, 서버 재시작 시 소실
//         - Redis: 분산 서버, 공유 가능, 영속성 옵션
//
// 백그라운드 정리 스레드: 주기적으로 만료 세션 삭제

#include "pvpserver/storage/in_memory_session_store.h"

#include <chrono>

namespace pvpserver {
namespace storage {

// [Order 1] 생성자 - 백그라운드 정리 스레드 시작
// [LEARN] std::thread 생성 시 멤버 함수 바인딩:
//         std::thread(&Class::Method, this)
InMemorySessionStore::InMemorySessionStore(int ttl_seconds,
                                           int cleanup_interval_seconds)
    : ttl_seconds_(ttl_seconds)
    , cleanup_interval_seconds_(cleanup_interval_seconds) {
    // 백그라운드 정리 스레드 시작
    cleanup_thread_ = std::thread(&InMemorySessionStore::CleanupLoop, this);
}

// 소멸자 - 스레드 정리 (RAII)
InMemorySessionStore::~InMemorySessionStore() {
    StopCleanup();
}

// StopCleanup - 스레드 안전하게 종료
// [LEARN] atomic 플래그로 루프 종료 신호, join()으로 대기
void InMemorySessionStore::StopCleanup() {
    running_.store(false, std::memory_order_release);
    if (cleanup_thread_.joinable()) {
        cleanup_thread_.join();  // 스레드 종료 대기
    }
}

// [Order 2] CleanupLoop - 백그라운드 정리 루프
// [LEARN] 주기적으로 만료 세션 삭제.
//         sleep_for: 비동기 대기 (CPU 점유 안 함)
void InMemorySessionStore::CleanupLoop() {
    while (running_.load(std::memory_order_acquire)) {
        std::this_thread::sleep_for(
            std::chrono::seconds(cleanup_interval_seconds_));

        if (running_.load(std::memory_order_acquire)) {
            CleanExpiredSessions();
        }
    }
}

// IsExpired - 만료 여부 확인
bool InMemorySessionStore::IsExpired(const SessionEntry& entry) const {
    return GetCurrentTimestamp() > entry.expiry_time;
}

// [Order 3] SaveSession - 세션 저장
// [LEARN] mutex로 동시성 보호.
//         player_to_session: 역방향 조회 맵 (플레이어 → 세션)
bool InMemorySessionStore::SaveSession(const std::string& session_id,
                                       const SessionData& data) {
    std::lock_guard<std::mutex> lock(mutex_);

    SessionEntry entry;
    entry.data = data;
    entry.expiry_time = GetCurrentTimestamp() + (ttl_seconds_ * 1000);

    // 기존 세션이 있으면 player_to_session 맵 정리
    auto existing = sessions_.find(session_id);
    if (existing != sessions_.end()) {
        player_to_session_.erase(existing->second.data.player_id);
    }

    sessions_[session_id] = entry;
    player_to_session_[data.player_id] = session_id;

    return true;
}

// [Order 4] GetSession - 세션 조회
// [LEARN] 조회 시 만료 체크 → 만료됐으면 삭제 후 nullopt 반환
std::optional<SessionData> InMemorySessionStore::GetSession(
    const std::string& session_id) {
    std::lock_guard<std::mutex> lock(mutex_);

    auto it = sessions_.find(session_id);
    if (it == sessions_.end()) {
        return std::nullopt;
    }

    if (IsExpired(it->second)) {
        // 만료된 세션 제거
        player_to_session_.erase(it->second.data.player_id);
        sessions_.erase(it);
        return std::nullopt;
    }

    return it->second.data;
}

bool InMemorySessionStore::DeleteSession(const std::string& session_id) {
    std::lock_guard<std::mutex> lock(mutex_);

    auto it = sessions_.find(session_id);
    if (it == sessions_.end()) {
        return false;
    }

    player_to_session_.erase(it->second.data.player_id);
    sessions_.erase(it);
    return true;
}

bool InMemorySessionStore::RefreshSession(const std::string& session_id) {
    std::lock_guard<std::mutex> lock(mutex_);

    auto it = sessions_.find(session_id);
    if (it == sessions_.end()) {
        return false;
    }

    if (IsExpired(it->second)) {
        player_to_session_.erase(it->second.data.player_id);
        sessions_.erase(it);
        return false;
    }

    // TTL 갱신
    it->second.expiry_time = GetCurrentTimestamp() + (ttl_seconds_ * 1000);
    it->second.data.Touch();
    return true;
}

std::vector<std::string> InMemorySessionStore::GetAllSessionIds() {
    std::lock_guard<std::mutex> lock(mutex_);

    std::vector<std::string> ids;
    ids.reserve(sessions_.size());

    auto now = GetCurrentTimestamp();
    for (const auto& [id, entry] : sessions_) {
        if (now <= entry.expiry_time) {
            ids.push_back(id);
        }
    }

    return ids;
}

std::optional<std::string> InMemorySessionStore::GetSessionByPlayerId(
    const std::string& player_id) {
    std::lock_guard<std::mutex> lock(mutex_);

    auto it = player_to_session_.find(player_id);
    if (it == player_to_session_.end()) {
        return std::nullopt;
    }

    // 세션이 유효한지 확인
    auto session_it = sessions_.find(it->second);
    if (session_it == sessions_.end() || IsExpired(session_it->second)) {
        player_to_session_.erase(it);
        if (session_it != sessions_.end()) {
            sessions_.erase(session_it);
        }
        return std::nullopt;
    }

    return it->second;
}

size_t InMemorySessionStore::GetActiveSessionCount() {
    std::lock_guard<std::mutex> lock(mutex_);

    size_t count = 0;
    auto now = GetCurrentTimestamp();
    for (const auto& [id, entry] : sessions_) {
        if (now <= entry.expiry_time) {
            ++count;
        }
    }
    return count;
}

bool InMemorySessionStore::SessionExists(const std::string& session_id) {
    std::lock_guard<std::mutex> lock(mutex_);

    auto it = sessions_.find(session_id);
    if (it == sessions_.end()) {
        return false;
    }
    return !IsExpired(it->second);
}

void InMemorySessionStore::CleanExpiredSessions() {
    std::lock_guard<std::mutex> lock(mutex_);

    auto now = GetCurrentTimestamp();
    std::vector<std::string> expired_ids;

    for (const auto& [id, entry] : sessions_) {
        if (now > entry.expiry_time) {
            expired_ids.push_back(id);
        }
    }

    for (const auto& id : expired_ids) {
        auto it = sessions_.find(id);
        if (it != sessions_.end()) {
            player_to_session_.erase(it->second.data.player_id);
            sessions_.erase(it);
        }
    }
}

}  // namespace storage
}  // namespace pvpserver
