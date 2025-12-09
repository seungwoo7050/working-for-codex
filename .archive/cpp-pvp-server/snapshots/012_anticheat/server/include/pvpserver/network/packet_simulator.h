#pragma once

#include <chrono>
#include <functional>
#include <memory>
#include <mutex>
#include <queue>
#include <random>
#include <vector>

namespace pvpserver {
namespace network {

/**
 * 네트워크 시뮬레이션 설정
 */
struct NetworkCondition {
    float packet_loss_percent = 0.0f;     // 0-100
    float latency_ms = 0.0f;               // 추가 지연
    float jitter_ms = 0.0f;                // 지연 변동성
    float duplicate_percent = 0.0f;        // 패킷 복제율
    float out_of_order_percent = 0.0f;     // 순서 뒤바뀜 비율

    static NetworkCondition Perfect() {
        return NetworkCondition{};
    }

    static NetworkCondition GoodWifi() {
        return NetworkCondition{0.5f, 20.0f, 5.0f, 0.0f, 0.0f};
    }

    static NetworkCondition PoorWifi() {
        return NetworkCondition{2.0f, 80.0f, 30.0f, 0.1f, 1.0f};
    }

    static NetworkCondition Mobile4G() {
        return NetworkCondition{1.0f, 50.0f, 20.0f, 0.0f, 0.5f};
    }

    static NetworkCondition Mobile3G() {
        return NetworkCondition{5.0f, 150.0f, 50.0f, 0.5f, 2.0f};
    }

    static NetworkCondition Terrible() {
        return NetworkCondition{10.0f, 300.0f, 100.0f, 1.0f, 5.0f};
    }
};

/**
 * 지연된 패킷 정보
 */
struct DelayedPacket {
    std::vector<uint8_t> data;
    std::chrono::steady_clock::time_point delivery_time;
    std::string destination;

    bool operator>(const DelayedPacket& other) const {
        return delivery_time > other.delivery_time;
    }
};

/**
 * 패킷 시뮬레이터
 * - 네트워크 조건 시뮬레이션 (손실, 지연, 지터, 복제, 순서 변경)
 * - 테스트 및 디버깅용
 */
class PacketSimulator {
public:
    using PacketHandler = std::function<void(const std::vector<uint8_t>&, const std::string&)>;

    explicit PacketSimulator(NetworkCondition condition = NetworkCondition::Perfect());
    ~PacketSimulator() = default;

    // 네트워크 조건 설정
    void SetCondition(const NetworkCondition& condition);
    NetworkCondition GetCondition() const;

    // 시뮬레이터 활성화/비활성화
    void SetEnabled(bool enabled);
    bool IsEnabled() const;

    // 패킷 전송 시뮬레이션
    // 반환값: 패킷이 즉시 전달되어야 하면 true
    bool SimulateSend(const std::vector<uint8_t>& data,
                      const std::string& destination,
                      PacketHandler on_deliver);

    // 지연된 패킷 처리 (틱마다 호출)
    void ProcessDelayedPackets(PacketHandler on_deliver);

    // 대기 중인 패킷 수
    size_t GetPendingPacketCount() const;

    // 통계
    struct Statistics {
        uint64_t total_packets = 0;
        uint64_t dropped_packets = 0;
        uint64_t delayed_packets = 0;
        uint64_t duplicated_packets = 0;
        uint64_t reordered_packets = 0;
    };
    Statistics GetStatistics() const;
    void ResetStatistics();

private:
    bool enabled_;
    NetworkCondition condition_;
    mutable std::mutex mutex_;

    std::mt19937 rng_;
    std::uniform_real_distribution<float> percent_dist_;
    std::normal_distribution<float> latency_dist_;

    std::priority_queue<DelayedPacket,
                       std::vector<DelayedPacket>,
                       std::greater<DelayedPacket>> delayed_queue_;

    Statistics stats_;

    bool ShouldDrop();
    bool ShouldDuplicate();
    bool ShouldReorder();
    std::chrono::steady_clock::time_point CalculateDeliveryTime();
};

}  // namespace network
}  // namespace pvpserver
