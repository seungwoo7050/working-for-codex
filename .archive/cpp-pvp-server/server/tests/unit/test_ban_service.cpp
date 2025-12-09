#include <gtest/gtest.h>

#include "pvpserver/anticheat/ban_service.h"

using namespace pvpserver::anticheat;

class BanServiceTest : public ::testing::Test {
protected:
    BanService ban_service_;
};

TEST_F(BanServiceTest, BanDurationToSeconds) {
    EXPECT_EQ(BanDurationToSeconds(BanDuration::WARNING), 0);
    EXPECT_EQ(BanDurationToSeconds(BanDuration::TEMP_1_HOUR), 3600);
    EXPECT_EQ(BanDurationToSeconds(BanDuration::TEMP_24_HOURS), 86400);
    EXPECT_EQ(BanDurationToSeconds(BanDuration::TEMP_7_DAYS), 604800);
    EXPECT_EQ(BanDurationToSeconds(BanDuration::TEMP_30_DAYS), 2592000);
    EXPECT_EQ(BanDurationToSeconds(BanDuration::PERMANENT), 0);
}

TEST_F(BanServiceTest, BanAndCheckPlayer) {
    BanRecord record;
    record.player_id = "player1";
    record.duration = BanDuration::TEMP_1_HOUR;
    record.reason = "speedhack";

    ban_service_.BanPlayer(record);

    auto result = ban_service_.CheckPlayer("player1");
    EXPECT_TRUE(result.is_banned);
    EXPECT_EQ(result.reason, "speedhack");
    EXPECT_GT(result.remaining_seconds, 0);
}

TEST_F(BanServiceTest, UnbannedPlayerNotBlocked) {
    auto result = ban_service_.CheckPlayer("clean_player");
    EXPECT_FALSE(result.is_banned);
}

TEST_F(BanServiceTest, UnbanPlayer) {
    BanRecord record;
    record.player_id = "player1";
    record.duration = BanDuration::PERMANENT;
    record.reason = "test";

    ban_service_.BanPlayer(record);
    EXPECT_TRUE(ban_service_.CheckPlayer("player1").is_banned);

    ban_service_.UnbanPlayer("player1");
    EXPECT_FALSE(ban_service_.CheckPlayer("player1").is_banned);
}

TEST_F(BanServiceTest, HardwareBan) {
    BanRecord record;
    record.player_id = "player1";
    record.hardware_id = "HWID-12345";
    record.duration = BanDuration::PERMANENT;
    record.reason = "ban evasion";

    ban_service_.BanPlayer(record);

    auto result = ban_service_.CheckHardwareId("HWID-12345");
    EXPECT_TRUE(result.is_banned);
    EXPECT_EQ(result.remaining_seconds, -1);  // 영구밴
}

TEST_F(BanServiceTest, IpBan) {
    BanRecord record;
    record.player_id = "player1";
    record.ip_address = "192.168.1.100";
    record.duration = BanDuration::TEMP_24_HOURS;
    record.reason = "multiple accounts";

    ban_service_.BanPlayer(record);

    auto result = ban_service_.CheckIpAddress("192.168.1.100");
    EXPECT_TRUE(result.is_banned);
}

TEST_F(BanServiceTest, CheckConnectionCombined) {
    // HWID 밴
    BanRecord hwid_ban;
    hwid_ban.player_id = "old_player";
    hwid_ban.hardware_id = "HWID-BANNED";
    hwid_ban.duration = BanDuration::PERMANENT;
    hwid_ban.reason = "permanent ban";

    ban_service_.BanPlayer(hwid_ban);

    // 새 계정으로 접속 시도하지만 HWID가 밴됨
    auto result =
        ban_service_.CheckConnection("new_player", "HWID-BANNED", "192.168.1.200");
    EXPECT_TRUE(result.is_banned);
}

TEST_F(BanServiceTest, BanHistory) {
    BanRecord ban1;
    ban1.player_id = "player1";
    ban1.duration = BanDuration::TEMP_1_HOUR;
    ban1.reason = "first offense";

    BanRecord ban2;
    ban2.player_id = "player1";
    ban2.duration = BanDuration::TEMP_24_HOURS;
    ban2.reason = "second offense";

    ban_service_.BanPlayer(ban1);
    ban_service_.UnbanPlayer("player1");
    ban_service_.BanPlayer(ban2);

    auto history = ban_service_.GetBanHistory("player1");
    EXPECT_EQ(history.size(), 2);
}

TEST_F(BanServiceTest, GetActiveBan) {
    BanRecord record;
    record.player_id = "player1";
    record.duration = BanDuration::TEMP_7_DAYS;
    record.reason = "active ban";

    ban_service_.BanPlayer(record);

    auto active = ban_service_.GetActiveBan("player1");
    ASSERT_TRUE(active.has_value());
    EXPECT_EQ(active->reason, "active ban");
}

TEST_F(BanServiceTest, DefaultPolicies) {
    // 속도핵 첫 번째 위반
    auto duration = ban_service_.GetBanDuration("speedhack", 1);
    EXPECT_EQ(duration, BanDuration::TEMP_7_DAYS);

    // 에임봇 첫 번째 위반
    duration = ban_service_.GetBanDuration("aimbot", 1);
    EXPECT_EQ(duration, BanDuration::TEMP_30_DAYS);

    // 에임봇 두 번째 위반
    duration = ban_service_.GetBanDuration("aimbot", 2);
    EXPECT_EQ(duration, BanDuration::PERMANENT);
}

TEST_F(BanServiceTest, CustomPolicy) {
    BanPolicy policy;
    policy.violation_type = "custom_cheat";
    policy.first_offense = BanDuration::TEMP_24_HOURS;
    policy.second_offense = BanDuration::TEMP_7_DAYS;
    policy.third_and_beyond = BanDuration::TEMP_30_DAYS;

    ban_service_.AddPolicy(policy);

    EXPECT_EQ(ban_service_.GetBanDuration("custom_cheat", 1), BanDuration::TEMP_24_HOURS);
    EXPECT_EQ(ban_service_.GetBanDuration("custom_cheat", 2), BanDuration::TEMP_7_DAYS);
    EXPECT_EQ(ban_service_.GetBanDuration("custom_cheat", 5), BanDuration::TEMP_30_DAYS);
}

TEST_F(BanServiceTest, ActiveBanCount) {
    BanRecord ban1;
    ban1.player_id = "player1";
    ban1.duration = BanDuration::TEMP_1_HOUR;
    ban1.reason = "test";

    BanRecord ban2;
    ban2.player_id = "player2";
    ban2.duration = BanDuration::PERMANENT;
    ban2.reason = "test";

    ban_service_.BanPlayer(ban1);
    ban_service_.BanPlayer(ban2);

    EXPECT_EQ(ban_service_.GetActiveBanCount(), 2);

    ban_service_.UnbanPlayer("player1");
    EXPECT_EQ(ban_service_.GetActiveBanCount(), 1);
}

TEST_F(BanServiceTest, GetAllActiveBans) {
    BanRecord ban1;
    ban1.player_id = "player1";
    ban1.duration = BanDuration::PERMANENT;
    ban1.reason = "cheat1";

    BanRecord ban2;
    ban2.player_id = "player2";
    ban2.duration = BanDuration::PERMANENT;
    ban2.reason = "cheat2";

    ban_service_.BanPlayer(ban1);
    ban_service_.BanPlayer(ban2);

    auto all_bans = ban_service_.GetAllActiveBans();
    EXPECT_EQ(all_bans.size(), 2);
}

// HardwareIdValidator 테스트
TEST_F(BanServiceTest, HWIDValidFormat) {
    EXPECT_TRUE(HardwareIdValidator::IsValidFormat("ABCD-1234-EFGH-5678"));
    EXPECT_TRUE(HardwareIdValidator::IsValidFormat("1234567890ABCDEF"));
    EXPECT_FALSE(HardwareIdValidator::IsValidFormat("short"));
    EXPECT_FALSE(HardwareIdValidator::IsValidFormat("invalid!@#$%chars"));
}

TEST_F(BanServiceTest, HWIDVirtualMachineDetection) {
    EXPECT_TRUE(HardwareIdValidator::IsVirtualMachine("VMWARE-12345678"));
    EXPECT_TRUE(HardwareIdValidator::IsVirtualMachine("vbox-something"));
    EXPECT_TRUE(HardwareIdValidator::IsVirtualMachine("QEMU-HYPERVISOR"));
    EXPECT_FALSE(HardwareIdValidator::IsVirtualMachine("GENUINE-HARDWARE-ID"));
}

TEST_F(BanServiceTest, HWIDHash) {
    std::string hwid = "TEST-HWID-12345678";
    std::string hashed = HardwareIdValidator::HashHWID(hwid);

    EXPECT_FALSE(hashed.empty());
    EXPECT_NE(hashed, hwid);

    // 동일 입력 -> 동일 해시
    EXPECT_EQ(hashed, HardwareIdValidator::HashHWID(hwid));
}
