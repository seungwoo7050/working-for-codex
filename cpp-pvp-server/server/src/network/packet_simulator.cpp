// [FILE]
// 목적: 네트워크 조건 시뮬레이터 - 패킷 손실, 지연, 순서 변경을 인위적으로 발생시켜 테스트
// - 관련 클론 가이드 단계: [v1.4.2-p2] 패킷 손실 시뮬레이션
// - 이 파일을 읽기 전에: design/v1.4.2-observability.md 참고
// - 학습 포인트:
//   [Order 1] NetworkCondition: 손실률, 지연, 지터, 중복, 순서변경 파라미터
//   [Order 2] 확률적 드롭: uniform_real_distribution으로 랜덤 패킷 손실
//   [Order 3] 지연 시뮬레이션: normal_distribution으로 지터가 있는 지연
// - [LEARN] std::random_device + mt19937: C의 rand()보다 품질 좋은 난수 생성
// - [Reader Notes] 부하 테스트 시 다양한 네트워크 환경(3G, WiFi, LTE)을 모방 가능
// - 다음에 읽을 파일: tests/performance/udp_load_test.cpp

#include "pvpserver/network/packet_simulator.h"

#include <algorithm>

namespace pvpserver {
namespace network {

PacketSimulator::PacketSimulator(NetworkCondition condition)
    : enabled_(false)
    , condition_(condition)
    , rng_(std::random_device{}())
    , percent_dist_(0.0f, 100.0f)
    , latency_dist_(0.0f, 1.0f) {}

void PacketSimulator::SetCondition(const NetworkCondition& condition) {
    std::lock_guard<std::mutex> lock(mutex_);
    condition_ = condition;

    // 지연 분포 재설정
    if (condition_.jitter_ms > 0.0f) {
        latency_dist_ = std::normal_distribution<float>(0.0f, condition_.jitter_ms);
    }
}

NetworkCondition PacketSimulator::GetCondition() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return condition_;
}

void PacketSimulator::SetEnabled(bool enabled) {
    std::lock_guard<std::mutex> lock(mutex_);
    enabled_ = enabled;
}

bool PacketSimulator::IsEnabled() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return enabled_;
}

bool PacketSimulator::ShouldDrop() {
    return percent_dist_(rng_) < condition_.packet_loss_percent;
}

bool PacketSimulator::ShouldDuplicate() {
    return percent_dist_(rng_) < condition_.duplicate_percent;
}

bool PacketSimulator::ShouldReorder() {
    return percent_dist_(rng_) < condition_.out_of_order_percent;
}

std::chrono::steady_clock::time_point PacketSimulator::CalculateDeliveryTime() {
    auto now = std::chrono::steady_clock::now();

    float delay_ms = condition_.latency_ms;

    // 지터 추가 (정규 분포)
    if (condition_.jitter_ms > 0.0f) {
        delay_ms += std::abs(latency_dist_(rng_));
    }

    // 순서 변경 시 추가 지연
    if (ShouldReorder()) {
        delay_ms += percent_dist_(rng_) * 2.0f;  // 0-200ms 추가
        stats_.reordered_packets++;
    }

    return now + std::chrono::milliseconds(static_cast<int>(delay_ms));
}

bool PacketSimulator::SimulateSend(const std::vector<uint8_t>& data,
                                   const std::string& destination,
                                   PacketHandler on_deliver) {
    std::lock_guard<std::mutex> lock(mutex_);

    stats_.total_packets++;

    if (!enabled_) {
        // 시뮬레이터 비활성화 시 즉시 전달
        if (on_deliver) {
            on_deliver(data, destination);
        }
        return true;
    }

    // 패킷 드롭 체크
    if (ShouldDrop()) {
        stats_.dropped_packets++;
        return false;  // 패킷 버려짐
    }

    // 복제 체크
    bool duplicate = ShouldDuplicate();
    if (duplicate) {
        stats_.duplicated_packets++;
    }

    // 지연 없으면 즉시 전달
    if (condition_.latency_ms <= 0.0f && condition_.jitter_ms <= 0.0f) {
        if (on_deliver) {
            on_deliver(data, destination);
            if (duplicate) {
                on_deliver(data, destination);  // 복제본
            }
        }
        return true;
    }

    // 지연된 패킷 큐에 추가
    DelayedPacket packet;
    packet.data = data;
    packet.destination = destination;
    packet.delivery_time = CalculateDeliveryTime();

    delayed_queue_.push(packet);
    stats_.delayed_packets++;

    // 복제본도 추가 (약간 다른 시간)
    if (duplicate) {
        packet.delivery_time = CalculateDeliveryTime();
        delayed_queue_.push(packet);
    }

    return false;  // 즉시 전달되지 않음
}

void PacketSimulator::ProcessDelayedPackets(PacketHandler on_deliver) {
    std::lock_guard<std::mutex> lock(mutex_);

    auto now = std::chrono::steady_clock::now();

    while (!delayed_queue_.empty()) {
        const auto& top = delayed_queue_.top();
        if (top.delivery_time > now) {
            break;  // 아직 전달 시간 안됨
        }

        if (on_deliver) {
            on_deliver(top.data, top.destination);
        }
        delayed_queue_.pop();
    }
}

size_t PacketSimulator::GetPendingPacketCount() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return delayed_queue_.size();
}

PacketSimulator::Statistics PacketSimulator::GetStatistics() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return stats_;
}

void PacketSimulator::ResetStatistics() {
    std::lock_guard<std::mutex> lock(mutex_);
    stats_ = Statistics{};
}

}  // namespace network
}  // namespace pvpserver
