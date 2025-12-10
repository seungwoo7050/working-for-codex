// [FILE]
// - 목적: 밴 서비스 (치터 제재 시스템)
// - 주요 역할: 플레이어/하드웨어/IP 밴 관리 및 에스컬레이션
// - 관련 클론 가이드 단계: [CG-02.10] 안티치트 시스템
// - 권장 읽는 순서: CheckPlayer → BanPlayer → SetBanPolicy → BanEscalation
//
// [LEARN] 밴 시스템의 3가지 축:
//         - 계정 밴: player_id 기준
//         - 하드웨어 밴: HWID로 다계정 방지
//         - IP 밴: 최후의 수단 (VPN 등으로 우회 가능)
//
// 에스컬레이션: 반복 위반 시 점점 강한 제재
//         경고 → 1시간 → 24시간 → 7일 → 30일 → 영구

#include "pvpserver/anticheat/ban_service.h"

#include <algorithm>
#include <chrono>
#include <functional>
#include <sstream>

namespace pvpserver::anticheat {

// [Order 1] BanDurationToSeconds - 밴 기간을 초 단위로 변환
int64_t BanDurationToSeconds(BanDuration duration) {
    switch (duration) {
        case BanDuration::WARNING:
            return 0;                    // 경고만
        case BanDuration::TEMP_1_HOUR:
            return 3600;                 // 1시간
        case BanDuration::TEMP_24_HOURS:
            return 86400;                // 24시간
        case BanDuration::TEMP_7_DAYS:
            return 604800;               // 7일
        case BanDuration::TEMP_30_DAYS:
            return 2592000;              // 30일
        case BanDuration::PERMANENT:
            return 0;                    // ban_end = 0으로 영구밴 표시
    }
    return 0;
}

// BanService 구현
BanService::BanService() { SetDefaultPolicies(); }

// [Order 2] CheckPlayer - 플레이어 밴 상태 확인
// [LEARN] 로그인/입장 시 호출하여 밴 여부 확인
BanCheckResult BanService::CheckPlayer(const std::string& player_id) {
    BanCheckResult result;

    auto it = player_bans_.find(player_id);
    if (it == player_bans_.end()) return result;

    // 활성 밴 찾기 (여러 밴 이력 중)
    for (const auto& ban : it->second) {
        if (!ban.is_active) continue;
        if (IsBanExpired(ban)) continue;  // 만료된 밴은 스킵

        result.is_banned = true;
        result.reason = ban.reason;
        result.duration = ban.duration;

        if (ban.duration == BanDuration::PERMANENT) {
            result.remaining_seconds = -1;  // 영구 표시
        } else {
            result.remaining_seconds = ban.ban_end - GetCurrentTimestamp();
        }
        break;  // 첫 번째 활성 밴만 반환
    }

    return result;
}

// CheckHardwareId - 하드웨어 ID 밴 확인
// [LEARN] 다계정(부캐) 밴 우회 방지. HWID = 머신 고유 식별자
BanCheckResult BanService::CheckHardwareId(const std::string& hardware_id) {
    BanCheckResult result;

    auto it = hwid_bans_.find(hardware_id);
    if (it == hwid_bans_.end()) return result;

    const auto& ban = it->second;
    if (!ban.is_active || IsBanExpired(ban)) return result;

    result.is_banned = true;
    result.reason = "Hardware banned: " + ban.reason;
    result.duration = ban.duration;
    if (ban.duration == BanDuration::PERMANENT) {
        result.remaining_seconds = -1;
    } else {
        result.remaining_seconds = ban.ban_end - GetCurrentTimestamp();
    }

    return result;
}

// CheckIpAddress - IP 주소 밴 확인
// [LEARN] 최후의 수단. VPN/프록시로 우회 가능하므로 제한적 사용
BanCheckResult BanService::CheckIpAddress(const std::string& ip_address) {
    BanCheckResult result;

    auto it = ip_bans_.find(ip_address);
    if (it == ip_bans_.end()) return result;

    const auto& ban = it->second;
    if (!ban.is_active || IsBanExpired(ban)) return result;

    result.is_banned = true;
    result.reason = "IP banned: " + ban.reason;
    result.duration = ban.duration;
    if (ban.duration == BanDuration::PERMANENT) {
        result.remaining_seconds = -1;
    } else {
        result.remaining_seconds = ban.ban_end - GetCurrentTimestamp();
    }

    return result;
}

// [Order 3] CheckConnection - 3중 밴 체크 (계정 + HWID + IP)
// [LEARN] 접속 시 모든 축으로 검사하여 우회 방지
BanCheckResult BanService::CheckConnection(const std::string& player_id,
                                           const std::string& hardware_id,
                                           const std::string& ip_address) {
    // 가장 심각한 밴 반환
    auto player_result = CheckPlayer(player_id);
    if (player_result.is_banned && player_result.duration == BanDuration::PERMANENT) {
        return player_result;
    }

    auto hwid_result = CheckHardwareId(hardware_id);
    if (hwid_result.is_banned && hwid_result.duration == BanDuration::PERMANENT) {
        return hwid_result;
    }

    auto ip_result = CheckIpAddress(ip_address);
    if (ip_result.is_banned && ip_result.duration == BanDuration::PERMANENT) {
        return ip_result;
    }

    // 영구밴 아니면 가장 긴 기간 반환
    if (player_result.is_banned) return player_result;
    if (hwid_result.is_banned) return hwid_result;
    if (ip_result.is_banned) return ip_result;

    return BanCheckResult{};
}

void BanService::BanPlayer(const BanRecord& record) {
    BanRecord ban = record;
    ban.ban_start = GetCurrentTimestamp();

    if (ban.duration == BanDuration::PERMANENT) {
        ban.ban_end = 0;
    } else {
        ban.ban_end = ban.ban_start + BanDurationToSeconds(ban.duration);
    }

    ban.is_active = true;

    // 플레이어 밴 추가
    player_bans_[ban.player_id].push_back(ban);

    // HWID 밴 (있는 경우)
    if (!ban.hardware_id.empty()) {
        hwid_bans_[ban.hardware_id] = ban;
    }

    // IP 밴 (있는 경우)
    if (!ban.ip_address.empty()) {
        ip_bans_[ban.ip_address] = ban;
    }
}

void BanService::UnbanPlayer(const std::string& player_id) {
    auto it = player_bans_.find(player_id);
    if (it == player_bans_.end()) return;

    for (auto& ban : it->second) {
        if (ban.is_active) {
            ban.is_active = false;

            // HWID, IP 밴도 해제
            if (!ban.hardware_id.empty()) {
                hwid_bans_.erase(ban.hardware_id);
            }
            if (!ban.ip_address.empty()) {
                ip_bans_.erase(ban.ip_address);
            }
        }
    }
}

std::vector<BanRecord> BanService::GetBanHistory(const std::string& player_id) {
    auto it = player_bans_.find(player_id);
    if (it == player_bans_.end()) return {};
    return it->second;
}

std::optional<BanRecord> BanService::GetActiveBan(const std::string& player_id) {
    auto it = player_bans_.find(player_id);
    if (it == player_bans_.end()) return std::nullopt;

    for (const auto& ban : it->second) {
        if (ban.is_active && !IsBanExpired(ban)) {
            return ban;
        }
    }
    return std::nullopt;
}

void BanService::AddPolicy(const BanPolicy& policy) { policies_.push_back(policy); }

void BanService::SetDefaultPolicies() {
    policies_.clear();

    // 속도핵
    policies_.push_back(
        {"speedhack", BanDuration::TEMP_7_DAYS, BanDuration::TEMP_30_DAYS, BanDuration::PERMANENT});

    // 에임봇
    policies_.push_back(
        {"aimbot", BanDuration::TEMP_30_DAYS, BanDuration::PERMANENT, BanDuration::PERMANENT});

    // 월핵
    policies_.push_back(
        {"wallhack", BanDuration::TEMP_7_DAYS, BanDuration::TEMP_30_DAYS, BanDuration::PERMANENT});

    // 텔레포트
    policies_.push_back(
        {"teleport", BanDuration::TEMP_24_HOURS, BanDuration::TEMP_7_DAYS, BanDuration::TEMP_30_DAYS});

    // 이상 통계
    policies_.push_back(
        {"anomaly", BanDuration::WARNING, BanDuration::TEMP_24_HOURS, BanDuration::TEMP_7_DAYS});
}

BanDuration BanService::GetBanDuration(const std::string& violation_type, int offense_count) {
    for (const auto& policy : policies_) {
        if (policy.violation_type == violation_type) {
            if (offense_count <= 1) return policy.first_offense;
            if (offense_count == 2) return policy.second_offense;
            return policy.third_and_beyond;
        }
    }
    // 정책 없으면 기본값
    return BanDuration::TEMP_24_HOURS;
}

void BanService::CleanupExpiredBans() {
    // 플레이어 밴 정리
    for (auto& [id, bans] : player_bans_) {
        for (auto& ban : bans) {
            if (ban.is_active && IsBanExpired(ban)) {
                ban.is_active = false;
            }
        }
    }

    // HWID 밴 정리
    for (auto it = hwid_bans_.begin(); it != hwid_bans_.end();) {
        if (IsBanExpired(it->second)) {
            it = hwid_bans_.erase(it);
        } else {
            ++it;
        }
    }

    // IP 밴 정리
    for (auto it = ip_bans_.begin(); it != ip_bans_.end();) {
        if (IsBanExpired(it->second)) {
            it = ip_bans_.erase(it);
        } else {
            ++it;
        }
    }
}

size_t BanService::GetActiveBanCount() const {
    size_t count = 0;
    for (const auto& [id, bans] : player_bans_) {
        for (const auto& ban : bans) {
            if (ban.is_active && !IsBanExpired(ban)) {
                count++;
                break;  // 플레이어당 한 번만
            }
        }
    }
    return count;
}

std::vector<BanRecord> BanService::GetAllActiveBans() const {
    std::vector<BanRecord> result;
    for (const auto& [id, bans] : player_bans_) {
        for (const auto& ban : bans) {
            if (ban.is_active && !IsBanExpired(ban)) {
                result.push_back(ban);
            }
        }
    }
    return result;
}

bool BanService::IsBanExpired(const BanRecord& record) const {
    if (!record.is_active) return true;
    if (record.duration == BanDuration::PERMANENT) return false;
    if (record.duration == BanDuration::WARNING) return true;
    return GetCurrentTimestamp() >= record.ban_end;
}

int64_t BanService::GetCurrentTimestamp() const {
    return std::chrono::duration_cast<std::chrono::seconds>(
               std::chrono::system_clock::now().time_since_epoch())
        .count();
}

// HardwareIdValidator 구현
bool HardwareIdValidator::IsValidFormat(const std::string& hwid) {
    // HWID는 최소 16자, 영숫자와 하이픈만 허용
    if (hwid.length() < 16) return false;
    for (char c : hwid) {
        if (!std::isalnum(static_cast<unsigned char>(c)) && c != '-') {
            return false;
        }
    }
    return true;
}

bool HardwareIdValidator::IsVirtualMachine(const std::string& hwid) {
    // 알려진 VM 패턴
    static const std::vector<std::string> vm_patterns = {
        "VMWARE",  "VIRTUALBOX", "VBOX",   "QEMU",
        "XEN",     "HYPERV",     "KVM",    "PARALLELS"};

    std::string upper_hwid = hwid;
    std::transform(upper_hwid.begin(), upper_hwid.end(), upper_hwid.begin(),
                   [](unsigned char c) { return std::toupper(c); });

    for (const auto& pattern : vm_patterns) {
        if (upper_hwid.find(pattern) != std::string::npos) {
            return true;
        }
    }
    return false;
}

std::string HardwareIdValidator::HashHWID(const std::string& hwid) {
    // 간단한 해시 (실제로는 SHA-256 사용)
    std::hash<std::string> hasher;
    size_t hash = hasher(hwid);

    std::stringstream ss;
    ss << std::hex << hash;
    return ss.str();
}

}  // namespace pvpserver::anticheat
