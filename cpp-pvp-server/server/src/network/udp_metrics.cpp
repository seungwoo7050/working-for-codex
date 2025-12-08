// [FILE]
// 목적: UDP 네트워크 메트릭 수집 - RTT, 지터, 패킷 손실률 측정
// - 관련 클론 가이드 단계: [v1.4.2-p1] UDP 메트릭 추가
// - 이 파일을 읽기 전에: design/v1.4.2-observability.md 참고
// - 학습 포인트:
//   [Order 1] RTT(Round Trip Time): 패킷 왕복 시간, 지연 보상의 핵심 데이터
//   [Order 2] Jitter: RFC 3550 방식의 지터 계산 (연속 RTT 차이의 평균)
//   [Order 3] Packet Loss: 손실률 계산으로 네트워크 품질 판단
// - [LEARN] atomic 연산: 멀티스레드 환경에서 안전한 통계 업데이트
// - [Reader Notes] Prometheus 메트릭과 연동하여 Grafana 대시보드에서 시각화
// - 다음에 읽을 파일: packet_simulator.cpp (테스트용 네트워크 조건 시뮬레이션)

#include "pvpserver/network/udp_metrics.h"

#include <algorithm>
#include <cmath>
#include <numeric>
#include <sstream>

namespace pvpserver {
namespace network {

// ClientMetrics 내부 계산 메서드

float UdpMetrics::ClientMetrics::CalculateAvgRtt() const {
    if (rtt_samples.empty()) {
        return 0.0f;
    }

    float sum = 0.0f;
    for (const auto& sample : rtt_samples) {
        sum += sample.rtt_ms;
    }
    return sum / static_cast<float>(rtt_samples.size());
}

float UdpMetrics::ClientMetrics::CalculateJitter() const {
    if (rtt_samples.size() < 2) {
        return 0.0f;
    }

    // RFC 3550 방식: 연속 RTT 차이의 평균
    float sum = 0.0f;
    for (size_t i = 1; i < rtt_samples.size(); ++i) {
        sum += std::abs(rtt_samples[i].rtt_ms - rtt_samples[i - 1].rtt_ms);
    }
    return sum / static_cast<float>(rtt_samples.size() - 1);
}

float UdpMetrics::ClientMetrics::CalculatePacketLoss() const {
    uint64_t total = packets_sent.load() + packets_lost.load();
    if (total == 0) {
        return 0.0f;
    }
    return 100.0f * static_cast<float>(packets_lost.load()) / static_cast<float>(total);
}

// UdpMetrics 구현

UdpMetrics::UdpMetrics(size_t rtt_sample_window)
    : rtt_sample_window_(rtt_sample_window) {}

UdpMetrics::ClientMetrics* UdpMetrics::GetOrCreateClient(const std::string& client_id) {
    std::lock_guard<std::mutex> lock(clients_mutex_);
    auto it = clients_.find(client_id);
    if (it == clients_.end()) {
        auto [inserted, _] = clients_.emplace(client_id, std::make_unique<ClientMetrics>());
        return inserted->second.get();
    }
    return it->second.get();
}

const UdpMetrics::ClientMetrics* UdpMetrics::GetClient(const std::string& client_id) const {
    std::lock_guard<std::mutex> lock(clients_mutex_);
    auto it = clients_.find(client_id);
    if (it == clients_.end()) {
        return nullptr;
    }
    return it->second.get();
}

void UdpMetrics::RecordPacketSent(const std::string& client_id, uint32_t sequence, size_t bytes) {
    auto* client = GetOrCreateClient(client_id);

    client->packets_sent.fetch_add(1, std::memory_order_relaxed);
    client->bytes_sent.fetch_add(bytes, std::memory_order_relaxed);

    // ACK 대기 목록에 추가
    std::lock_guard<std::mutex> lock(client->rtt_mutex);
    client->pending_acks[sequence] = std::chrono::steady_clock::now();
}

void UdpMetrics::RecordPacketReceived(const std::string& client_id, uint32_t sequence, size_t bytes) {
    auto* client = GetOrCreateClient(client_id);

    client->packets_received.fetch_add(1, std::memory_order_relaxed);
    client->bytes_received.fetch_add(bytes, std::memory_order_relaxed);
}

void UdpMetrics::RecordAck(const std::string& client_id, uint32_t sequence) {
    auto* client = GetOrCreateClient(client_id);

    std::lock_guard<std::mutex> lock(client->rtt_mutex);
    auto it = client->pending_acks.find(sequence);
    if (it == client->pending_acks.end()) {
        return;  // 이미 처리됨 또는 알 수 없는 시퀀스
    }

    auto now = std::chrono::steady_clock::now();
    float rtt_ms = std::chrono::duration<float, std::milli>(now - it->second).count();

    // RTT 샘플 저장
    RttSample sample;
    sample.sequence = sequence;
    sample.sent_time = it->second;
    sample.recv_time = now;
    sample.rtt_ms = rtt_ms;

    client->rtt_samples.push_back(sample);

    // 윈도우 크기 초과 시 오래된 샘플 제거
    if (client->rtt_samples.size() > rtt_sample_window_) {
        client->rtt_samples.erase(client->rtt_samples.begin());
    }

    client->pending_acks.erase(it);
}

void UdpMetrics::RecordPacketLoss(const std::string& client_id, uint32_t sequence) {
    auto* client = GetOrCreateClient(client_id);

    client->packets_lost.fetch_add(1, std::memory_order_relaxed);

    // 대기 목록에서 제거
    std::lock_guard<std::mutex> lock(client->rtt_mutex);
    client->pending_acks.erase(sequence);
}

ConnectionQuality UdpMetrics::GetConnectionQuality(const std::string& client_id) const {
    const auto* client = GetClient(client_id);
    if (!client) {
        return ConnectionQuality{};
    }

    ConnectionQuality quality;
    quality.packets_sent = client->packets_sent.load();
    quality.packets_received = client->packets_received.load();
    quality.packets_lost = client->packets_lost.load();
    quality.bytes_sent = client->bytes_sent.load();
    quality.bytes_received = client->bytes_received.load();

    {
        std::lock_guard<std::mutex> lock(const_cast<std::mutex&>(client->rtt_mutex));
        quality.avg_rtt_ms = client->CalculateAvgRtt();
        quality.jitter_ms = client->CalculateJitter();
    }
    quality.packet_loss_percent = client->CalculatePacketLoss();

    return quality;
}

ConnectionQuality UdpMetrics::GetAggregatedMetrics() const {
    ConnectionQuality total;
    size_t client_count = 0;
    float total_rtt = 0.0f;
    float total_jitter = 0.0f;

    std::lock_guard<std::mutex> lock(clients_mutex_);
    for (const auto& [id, client] : clients_) {
        total.packets_sent += client->packets_sent.load();
        total.packets_received += client->packets_received.load();
        total.packets_lost += client->packets_lost.load();
        total.bytes_sent += client->bytes_sent.load();
        total.bytes_received += client->bytes_received.load();

        {
            std::lock_guard<std::mutex> rtt_lock(const_cast<std::mutex&>(client->rtt_mutex));
            total_rtt += client->CalculateAvgRtt();
            total_jitter += client->CalculateJitter();
        }
        ++client_count;
    }

    if (client_count > 0) {
        total.avg_rtt_ms = total_rtt / static_cast<float>(client_count);
        total.jitter_ms = total_jitter / static_cast<float>(client_count);
    }

    uint64_t total_packets = total.packets_sent + total.packets_lost;
    if (total_packets > 0) {
        total.packet_loss_percent = 100.0f * static_cast<float>(total.packets_lost) / static_cast<float>(total_packets);
    }

    return total;
}

std::string UdpMetrics::ExportPrometheus() const {
    std::ostringstream oss;

    auto aggregated = GetAggregatedMetrics();

    // 서버 전체 메트릭
    oss << "# HELP pvp_udp_packets_sent_total Total UDP packets sent\n";
    oss << "# TYPE pvp_udp_packets_sent_total counter\n";
    oss << "pvp_udp_packets_sent_total " << aggregated.packets_sent << "\n\n";

    oss << "# HELP pvp_udp_packets_received_total Total UDP packets received\n";
    oss << "# TYPE pvp_udp_packets_received_total counter\n";
    oss << "pvp_udp_packets_received_total " << aggregated.packets_received << "\n\n";

    oss << "# HELP pvp_udp_packets_lost_total Total UDP packets lost\n";
    oss << "# TYPE pvp_udp_packets_lost_total counter\n";
    oss << "pvp_udp_packets_lost_total " << aggregated.packets_lost << "\n\n";

    oss << "# HELP pvp_udp_bytes_sent_total Total bytes sent via UDP\n";
    oss << "# TYPE pvp_udp_bytes_sent_total counter\n";
    oss << "pvp_udp_bytes_sent_total " << aggregated.bytes_sent << "\n\n";

    oss << "# HELP pvp_udp_bytes_received_total Total bytes received via UDP\n";
    oss << "# TYPE pvp_udp_bytes_received_total counter\n";
    oss << "pvp_udp_bytes_received_total " << aggregated.bytes_received << "\n\n";

    oss << "# HELP pvp_udp_packet_loss_percent Packet loss percentage\n";
    oss << "# TYPE pvp_udp_packet_loss_percent gauge\n";
    oss << "pvp_udp_packet_loss_percent " << aggregated.packet_loss_percent << "\n\n";

    oss << "# HELP pvp_udp_rtt_ms Average round-trip time in milliseconds\n";
    oss << "# TYPE pvp_udp_rtt_ms gauge\n";
    oss << "pvp_udp_rtt_ms " << aggregated.avg_rtt_ms << "\n\n";

    oss << "# HELP pvp_udp_jitter_ms Network jitter in milliseconds\n";
    oss << "# TYPE pvp_udp_jitter_ms gauge\n";
    oss << "pvp_udp_jitter_ms " << aggregated.jitter_ms << "\n\n";

    // 클라이언트별 메트릭
    std::lock_guard<std::mutex> lock(clients_mutex_);
    for (const auto& [client_id, client] : clients_) {
        std::string safe_id = client_id;
        // 특수 문자 제거 (Prometheus 라벨 안전)
        std::replace_if(safe_id.begin(), safe_id.end(), 
                       [](char c) { return !std::isalnum(c) && c != '_'; }, '_');

        oss << "pvp_udp_client_rtt_ms{client=\"" << safe_id << "\"} ";
        {
            std::lock_guard<std::mutex> rtt_lock(const_cast<std::mutex&>(client->rtt_mutex));
            oss << client->CalculateAvgRtt();
        }
        oss << "\n";
    }

    return oss.str();
}

void UdpMetrics::ClearClient(const std::string& client_id) {
    std::lock_guard<std::mutex> lock(clients_mutex_);
    clients_.erase(client_id);
}

void UdpMetrics::Reset() {
    std::lock_guard<std::mutex> lock(clients_mutex_);
    clients_.clear();
}

// 글로벌 인스턴스
UdpMetrics& GetGlobalUdpMetrics() {
    static UdpMetrics instance;
    return instance;
}

}  // namespace network
}  // namespace pvpserver
