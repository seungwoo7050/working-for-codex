// [FILE]
// - 목적: 세션 데이터 및 기본 직렬화
// - 주요 역할: SessionData 구조체 정의, JSON 직렬화/역직렬화
// - 관련 클론 가이드 단계: [CG-02.00] 분산 시스템 - 세션 관리
// - 권장 읽는 순서: SessionData → Serialize → Deserialize
//
// [LEARN] 세션 데이터:
//         - player_id: 고유 식별자
//         - server_id: 현재 접속 중인 서버
//         - elo_rating: 현재 ELO 점수 (캐시)
//         - match_id: 현재 참여 중인 매치
//
// 직렬화: 메모리 → 저장 가능한 형태 (JSON, Protobuf 등)
// 역직렬화: 저장 형태 → 메모리

#include "pvpserver/storage/session_store.h"

#include <chrono>
#include <random>
#include <sstream>
#include <iomanip>

namespace pvpserver {
namespace storage {

// [Order 1] Serialize - JSON 형식 직렬화
// [LEARN] 간단한 JSON 생성. 실제로는 nlohmann/json 권장:
//         json j = {{"player_id", player_id}, {"elo_rating", elo_rating}};
//         return j.dump();
std::string SessionData::Serialize() const {
    std::ostringstream oss;
    oss << "{";
    oss << "\"player_id\":\"" << player_id << "\",";
    oss << "\"player_name\":\"" << player_name << "\",";
    oss << "\"server_id\":\"" << server_id << "\",";
    oss << "\"created_at\":" << created_at << ",";
    oss << "\"last_activity\":" << last_activity << ",";
    oss << "\"elo_rating\":" << elo_rating << ",";
    oss << "\"match_id\":\"" << match_id << "\"";
    oss << "}";
    return oss.str();
}

// [Order 2] Deserialize - JSON 역직렬화
// [LEARN] 문자열에서 값 추출. 실제로는:
//         auto j = json::parse(data);
//         session.player_id = j["player_id"];
std::optional<SessionData> SessionData::Deserialize(const std::string& data) {
    // 간단한 JSON 파싱 (실제로는 nlohmann/json 등 사용 권장)
    SessionData session;

    // 문자열 값 추출 헬퍼 람다
    auto extract_string = [&data](const std::string& key) -> std::string {
        std::string search = "\"" + key + "\":\"";
        auto pos = data.find(search);
        if (pos == std::string::npos) return "";
        pos += search.length();
        auto end = data.find("\"", pos);
        if (end == std::string::npos) return "";
        return data.substr(pos, end - pos);
    };

    // 정수 값 추출 헬퍼
    auto extract_int64 = [&data](const std::string& key) -> int64_t {
        std::string search = "\"" + key + "\":";
        auto pos = data.find(search);
        if (pos == std::string::npos) return 0;
        pos += search.length();
        auto end = data.find_first_of(",}", pos);
        if (end == std::string::npos) return 0;
        try {
            return std::stoll(data.substr(pos, end - pos));
        } catch (...) {
            return 0;
        }
    };

    auto extract_int = [&data](const std::string& key) -> int {
        std::string search = "\"" + key + "\":";
        auto pos = data.find(search);
        if (pos == std::string::npos) return 0;
        pos += search.length();
        auto end = data.find_first_of(",}", pos);
        if (end == std::string::npos) return 0;
        try {
            return std::stoi(data.substr(pos, end - pos));
        } catch (...) {
            return 0;
        }
    };

    // 필드 추출
    session.player_id = extract_string("player_id");
    session.player_name = extract_string("player_name");
    session.server_id = extract_string("server_id");
    session.created_at = extract_int64("created_at");
    session.last_activity = extract_int64("last_activity");
    session.elo_rating = extract_int("elo_rating");
    session.match_id = extract_string("match_id");

    // 필수 필드 검증
    if (session.player_id.empty()) {
        return std::nullopt;
    }

    return session;
}

void SessionData::Touch() {
    last_activity = GetCurrentTimestamp();
}

bool SessionData::IsValid() const {
    return !player_id.empty();
}

std::string GenerateSessionId() {
    static std::random_device rd;
    static std::mt19937_64 gen(rd());
    static std::uniform_int_distribution<uint64_t> dis;

    std::ostringstream oss;
    oss << std::hex << std::setfill('0');
    oss << std::setw(16) << dis(gen);
    oss << std::setw(16) << dis(gen);
    return oss.str();
}

int64_t GetCurrentTimestamp() {
    return std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()
    ).count();
}

}  // namespace storage
}  // namespace pvpserver
