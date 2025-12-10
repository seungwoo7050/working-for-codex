// [FILE]
// - 목적: Redis 세션 저장소 (분산 세션 관리)
// - 주요 역할: 여러 서버 간 세션 상태 공유
// - 관련 클론 가이드 단계: [CG-02.00] 분산 시스템 - 세션 스토어
// - 권장 읽는 순서: SaveSession → GetSession → MakeSessionKey
//
// [LEARN] Redis 특징 (세션 저장용):
//         - 인메모리: 빠른 읽기/쓰기 (< 1ms)
//         - TTL 지원: 자동 만료 (세션 타임아웃)
//         - 분산: 여러 서버가 동일 세션 접근
//         - 데이터 구조: String, Set, Hash 등 다양한 타입
//
// 이 구현은 스텁(stub)입니다. 실제로는 redis-plus-plus 라이브러리 필요.

#include "pvpserver/storage/redis_session_store.h"

// Note: 이 구현은 redis-plus-plus 또는 hiredis 라이브러리가 필요합니다.
// 현재는 스텁 구현으로, 실제 Redis 연동은 의존성 추가 후 구현됩니다.

namespace pvpserver {
namespace storage {

// [Order 1] RedisClusterClient 스텁
// [LEARN] 실제 구현 시 redis-plus-plus 사용:
//         #include <sw/redis++/redis++.h>
//         auto redis = sw::redis::RedisCluster("redis://127.0.0.1:6379");
class RedisClusterClient {
public:
    explicit RedisClusterClient(const RedisConfig& /*config*/) {}
    ~RedisClusterClient() = default;

    // SET key value EX ttl
    bool set(const std::string& /*key*/, const std::string& /*value*/,
             int /*ttl_seconds*/) {
        return true;
    }

    // GET key
    std::optional<std::string> get(const std::string& /*key*/) {
        return std::nullopt;
    }

    // DEL key
    bool del(const std::string& /*key*/) {
        return true;
    }

    // EXPIRE key seconds
    bool expire(const std::string& /*key*/, int /*seconds*/) {
        return true;
    }

    // EXISTS key
    bool exists(const std::string& /*key*/) {
        return false;
    }

    // SCAN 0 MATCH pattern
    std::vector<std::string> scan(const std::string& /*pattern*/) {
        return {};
    }

    // SADD key member (Set에 추가)
    bool sadd(const std::string& /*key*/, const std::string& /*member*/) {
        return true;
    }

    // SREM key member (Set에서 제거)
    bool srem(const std::string& /*key*/, const std::string& /*member*/) {
        return true;
    }

    // SMEMBERS key (Set 전체 조회)
    std::vector<std::string> smembers(const std::string& /*key*/) {
        return {};
    }

    // SCARD key (Set 크기)
    size_t scard(const std::string& /*key*/) {
        return 0;
    }

    // PING - 연결 확인
    bool ping() {
        return true;
    }
};

// [Order 2] RedisSessionStore 생성자
RedisSessionStore::RedisSessionStore(const RedisConfig& config)
    : config_(config)
    , client_(std::make_unique<RedisClusterClient>(config)) {}

RedisSessionStore::~RedisSessionStore() = default;

// 키 생성 헬퍼 - 네임스페이스 충돌 방지
std::string RedisSessionStore::MakeSessionKey(const std::string& session_id) const {
    return std::string(SESSION_KEY_PREFIX) + session_id;  // "session:" + id
}

std::string RedisSessionStore::MakePlayerSessionKey(const std::string& player_id) const {
    return std::string(PLAYER_SESSION_PREFIX) + player_id;  // "player_session:" + id
}

// [Order 3] SaveSession - 세션 저장
// [LEARN] 여러 키를 함께 저장:
//         - session:{id} → 세션 데이터 (JSON)
//         - player_session:{player_id} → session_id (역방향 조회용)
//         - sessions (Set) → 활성 세션 ID 집합
bool RedisSessionStore::SaveSession(const std::string& session_id,
                                    const SessionData& data) {
    if (!client_) return false;

    std::string key = MakeSessionKey(session_id);
    std::string value = data.Serialize();  // JSON 직렬화

    // TTL 설정으로 자동 만료
    bool success = client_->set(key, value, config_.session_ttl_seconds);
    if (success) {
        // player_id → session_id 매핑 저장 (플레이어로 세션 찾기용)
        client_->set(MakePlayerSessionKey(data.player_id),
                    session_id, config_.session_ttl_seconds);
        // 세션 집합에 추가 (활성 세션 목록 관리)
        client_->sadd(SESSION_SET_KEY, session_id);
    }
    return success;
}

// [Order 4] GetSession - 세션 조회
std::optional<SessionData> RedisSessionStore::GetSession(
    const std::string& session_id) {
    if (!client_) return std::nullopt;

    std::string key = MakeSessionKey(session_id);
    auto value = client_->get(key);

    if (!value) return std::nullopt;

    return SessionData::Deserialize(*value);
}

bool RedisSessionStore::DeleteSession(const std::string& session_id) {
    if (!client_) return false;

    // 먼저 세션 데이터를 가져와서 player_id 확인
    auto session = GetSession(session_id);
    if (session) {
        client_->del(MakePlayerSessionKey(session->player_id));
    }

    client_->srem(SESSION_SET_KEY, session_id);
    return client_->del(MakeSessionKey(session_id));
}

bool RedisSessionStore::RefreshSession(const std::string& session_id) {
    if (!client_) return false;

    std::string key = MakeSessionKey(session_id);
    return client_->expire(key, config_.session_ttl_seconds);
}

std::vector<std::string> RedisSessionStore::GetAllSessionIds() {
    if (!client_) return {};

    return client_->smembers(SESSION_SET_KEY);
}

std::optional<std::string> RedisSessionStore::GetSessionByPlayerId(
    const std::string& player_id) {
    if (!client_) return std::nullopt;

    return client_->get(MakePlayerSessionKey(player_id));
}

size_t RedisSessionStore::GetActiveSessionCount() {
    if (!client_) return 0;

    return client_->scard(SESSION_SET_KEY);
}

bool RedisSessionStore::SessionExists(const std::string& session_id) {
    if (!client_) return false;

    return client_->exists(MakeSessionKey(session_id));
}

bool RedisSessionStore::IsConnected() const {
    if (!client_) return false;

    return client_->ping();
}

void RedisSessionStore::Reconnect() {
    client_ = std::make_unique<RedisClusterClient>(config_);
}

}  // namespace storage
}  // namespace pvpserver
