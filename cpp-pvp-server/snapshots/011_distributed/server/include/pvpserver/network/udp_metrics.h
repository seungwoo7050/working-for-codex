#pragma once

#include <atomic>
#include <chrono>
#include <cstdint>
#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>

namespace pvpserver {
namespace network {

/**
 * UDP 통신 품질 메트릭
 */
struct ConnectionQuality {
    float packet_loss_percent = 0.0f;    // 패킷 손실률
    float avg_rtt_ms = 0.0f;             // 평균 RTT
    float jitter_ms = 0.0f;              // 지터 (RTT 변동성)
    uint64_t packets_sent = 0;
    uint64_t packets_received = 0;
    uint64_t packets_lost = 0;
    uint64_t bytes_sent = 0;
    uint64_t bytes_received = 0;
};

/**
 * RTT 샘플 (라운드트립 시간 측정용)
 */
struct RttSample {
    uint32_t sequence;
    std::chrono::steady_clock::time_point sent_time;
    std::chrono::steady_clock::time_point recv_time;
    float rtt_ms;
};

/**
 * UDP 메트릭 수집기
 * - 클라이언트별 연결 품질 추적
 * - 패킷 손실/지터/RTT 측정
 * - Prometheus 형식 출력
 */
class UdpMetrics {
public:
    explicit UdpMetrics(size_t rtt_sample_window = 100);
    ~UdpMetrics() = default;

    // 패킷 송신 기록
    void RecordPacketSent(const std::string& client_id, uint32_t sequence, size_t bytes);

    // 패킷 수신 기록
    void RecordPacketReceived(const std::string& client_id, uint32_t sequence, size_t bytes);

    // ACK 수신 (RTT 계산)
    void RecordAck(const std::string& client_id, uint32_t sequence);

    // 패킷 손실 감지
    void RecordPacketLoss(const std::string& client_id, uint32_t sequence);

    // 클라이언트별 연결 품질 조회
    ConnectionQuality GetConnectionQuality(const std::string& client_id) const;

    // 전체 서버 메트릭 조회
    ConnectionQuality GetAggregatedMetrics() const;

    // Prometheus 형식 문자열 출력
    std::string ExportPrometheus() const;

    // 클라이언트 메트릭 초기화 (연결 종료 시)
    void ClearClient(const std::string& client_id);

    // 전체 메트릭 리셋
    void Reset();

private:
    struct ClientMetrics {
        std::atomic<uint64_t> packets_sent{0};
        std::atomic<uint64_t> packets_received{0};
        std::atomic<uint64_t> packets_lost{0};
        std::atomic<uint64_t> bytes_sent{0};
        std::atomic<uint64_t> bytes_received{0};

        std::mutex rtt_mutex;
        std::vector<RttSample> rtt_samples;
        std::unordered_map<uint32_t, std::chrono::steady_clock::time_point> pending_acks;

        float CalculateAvgRtt() const;
        float CalculateJitter() const;
        float CalculatePacketLoss() const;
    };

    size_t rtt_sample_window_;
    mutable std::mutex clients_mutex_;
    std::unordered_map<std::string, std::unique_ptr<ClientMetrics>> clients_;

    ClientMetrics* GetOrCreateClient(const std::string& client_id);
    const ClientMetrics* GetClient(const std::string& client_id) const;
};

/**
 * 글로벌 UDP 메트릭 인스턴스 접근
 */
UdpMetrics& GetGlobalUdpMetrics();

}  // namespace network
}  // namespace pvpserver
