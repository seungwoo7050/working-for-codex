#pragma once

#include <cstdint>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace pvpserver::anticheat {

// 밴 기간
enum class BanDuration {
    WARNING,           // 경고만
    TEMP_1_HOUR,
    TEMP_24_HOURS,
    TEMP_7_DAYS,
    TEMP_30_DAYS,
    PERMANENT
};

// 밴 기간을 초로 변환
int64_t BanDurationToSeconds(BanDuration duration);

// 밴 기록
struct BanRecord {
    std::string player_id;
    std::string hardware_id;
    std::string ip_address;
    BanDuration duration;
    int64_t ban_start;
    int64_t ban_end;  // 0 = 영구밴
    std::string reason;
    std::vector<std::string> evidence;  // 리플레이 ID 등
    bool is_active = true;
};

// 밴 정책
struct BanPolicy {
    std::string violation_type;
    BanDuration first_offense;
    BanDuration second_offense;
    BanDuration third_and_beyond;
};

// 밴 확인 결과
struct BanCheckResult {
    bool is_banned = false;
    std::string reason;
    int64_t remaining_seconds = 0;
    BanDuration duration = BanDuration::WARNING;
};

// 밴 서비스
class BanService {
public:
    BanService();
    virtual ~BanService() = default;

    // 밴 확인
    BanCheckResult CheckPlayer(const std::string& player_id);
    BanCheckResult CheckHardwareId(const std::string& hardware_id);
    BanCheckResult CheckIpAddress(const std::string& ip_address);

    // 복합 확인 (플레이어 ID, HWID, IP 모두 확인)
    BanCheckResult CheckConnection(const std::string& player_id, const std::string& hardware_id,
                                   const std::string& ip_address);

    // 밴 적용
    void BanPlayer(const BanRecord& record);
    void UnbanPlayer(const std::string& player_id);

    // 이력 조회
    std::vector<BanRecord> GetBanHistory(const std::string& player_id);
    std::optional<BanRecord> GetActiveBan(const std::string& player_id);

    // 정책 관리
    void AddPolicy(const BanPolicy& policy);
    void SetDefaultPolicies();
    BanDuration GetBanDuration(const std::string& violation_type, int offense_count);

    // 만료된 밴 정리
    void CleanupExpiredBans();

    // 통계
    size_t GetActiveBanCount() const;
    std::vector<BanRecord> GetAllActiveBans() const;

private:
    // 플레이어 ID -> 밴 기록들
    std::unordered_map<std::string, std::vector<BanRecord>> player_bans_;

    // HWID -> 밴 기록
    std::unordered_map<std::string, BanRecord> hwid_bans_;

    // IP -> 밴 기록
    std::unordered_map<std::string, BanRecord> ip_bans_;

    // 정책
    std::vector<BanPolicy> policies_;

    bool IsBanExpired(const BanRecord& record) const;
    int64_t GetCurrentTimestamp() const;
};

// HWID 생성 유틸리티 (클라이언트에서 전송받는 값 검증용)
class HardwareIdValidator {
public:
    // HWID 형식 검증
    static bool IsValidFormat(const std::string& hwid);

    // 알려진 가상 머신 HWID 패턴 탐지
    static bool IsVirtualMachine(const std::string& hwid);

    // HWID 해싱 (저장용)
    static std::string HashHWID(const std::string& hwid);
};

}  // namespace pvpserver::anticheat
