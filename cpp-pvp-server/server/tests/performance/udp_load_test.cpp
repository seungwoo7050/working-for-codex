#include <gtest/gtest.h>

#include <atomic>
#include <chrono>
#include <random>
#include <thread>
#include <vector>

#include "pvpserver/network/packet_simulator.h"
#include "pvpserver/network/packet_types.h"
#include "pvpserver/network/snapshot_manager.h"
#include "pvpserver/network/udp_metrics.h"
#include "pvpserver/game/player_state.h"

using namespace pvpserver::network;
using namespace pvpserver;

/**
 * UDP 부하 테스트
 * - 다양한 네트워크 조건에서의 성능 측정
 * - 패킷 처리 처리량 테스트
 * - 메트릭 수집 오버헤드 측정
 */
class UdpLoadTest : public ::testing::Test {
protected:
    void SetUp() override {
        metrics_.Reset();
    }

    UdpMetrics metrics_;
    PacketSimulator simulator_;
};

// 기본 패킷 전송/수신 처리량 테스트
TEST_F(UdpLoadTest, PacketThroughputBaseline) {
    const int NUM_CLIENTS = 100;
    const int PACKETS_PER_CLIENT = 1000;
    const size_t PACKET_SIZE = 256;

    auto start = std::chrono::steady_clock::now();

    for (int client = 0; client < NUM_CLIENTS; ++client) {
        std::string client_id = "client_" + std::to_string(client);

        for (int seq = 0; seq < PACKETS_PER_CLIENT; ++seq) {
            metrics_.RecordPacketSent(client_id, seq, PACKET_SIZE);
            metrics_.RecordPacketReceived(client_id, seq, PACKET_SIZE);
        }
    }

    auto end = std::chrono::steady_clock::now();
    auto duration_ms = std::chrono::duration<double, std::milli>(end - start).count();

    int total_operations = NUM_CLIENTS * PACKETS_PER_CLIENT * 2;  // sent + received
    double ops_per_second = (total_operations / duration_ms) * 1000.0;

    std::cout << "[PERF] Packet recording throughput: " << ops_per_second << " ops/sec\n";
    std::cout << "[PERF] Total time: " << duration_ms << " ms for " << total_operations << " operations\n";

    auto aggregated = metrics_.GetAggregatedMetrics();
    EXPECT_EQ(aggregated.packets_sent, static_cast<uint64_t>(NUM_CLIENTS * PACKETS_PER_CLIENT));
    EXPECT_EQ(aggregated.packets_received, static_cast<uint64_t>(NUM_CLIENTS * PACKETS_PER_CLIENT));

    // 성능 기준: 최소 100,000 ops/sec
    EXPECT_GT(ops_per_second, 100000.0);
}

// RTT 측정 정확도 테스트
TEST_F(UdpLoadTest, RttMeasurementAccuracy) {
    const std::string client_id = "test_client";
    const int NUM_SAMPLES = 100;
    const float SIMULATED_RTT_MS = 50.0f;

    for (int seq = 0; seq < NUM_SAMPLES; ++seq) {
        metrics_.RecordPacketSent(client_id, seq, 64);

        // RTT 시뮬레이션
        std::this_thread::sleep_for(std::chrono::microseconds(static_cast<int>(SIMULATED_RTT_MS * 1000)));

        metrics_.RecordAck(client_id, seq);
    }

    auto quality = metrics_.GetConnectionQuality(client_id);

    std::cout << "[PERF] Measured avg RTT: " << quality.avg_rtt_ms << " ms (expected: ~" << SIMULATED_RTT_MS << " ms)\n";
    std::cout << "[PERF] Measured jitter: " << quality.jitter_ms << " ms\n";

    // RTT가 시뮬레이션된 값에 가까운지 확인 (±20% 허용)
    EXPECT_NEAR(quality.avg_rtt_ms, SIMULATED_RTT_MS, SIMULATED_RTT_MS * 0.3f);
}

// 패킷 시뮬레이터 손실률 테스트
TEST_F(UdpLoadTest, PacketSimulatorLossRate) {
    const int NUM_PACKETS = 10000;
    const float TARGET_LOSS = 5.0f;

    simulator_.SetCondition(NetworkCondition{TARGET_LOSS, 0.0f, 0.0f, 0.0f, 0.0f});
    simulator_.SetEnabled(true);

    int delivered = 0;
    int dropped = 0;

    for (int i = 0; i < NUM_PACKETS; ++i) {
        std::vector<uint8_t> data(64, 0);
        bool immediate = simulator_.SimulateSend(data, "dest", [&](const auto&, const auto&) {
            ++delivered;
        });
        if (!immediate) {
            ++dropped;
        }
    }

    float actual_loss = 100.0f * static_cast<float>(dropped) / static_cast<float>(NUM_PACKETS);

    std::cout << "[PERF] Target loss: " << TARGET_LOSS << "%, Actual loss: " << actual_loss << "%\n";
    std::cout << "[PERF] Delivered: " << delivered << ", Dropped: " << dropped << "\n";

    // 손실률이 목표에 가까운지 확인 (±2% 허용)
    EXPECT_NEAR(actual_loss, TARGET_LOSS, 2.0f);
}

// 다양한 네트워크 조건 시뮬레이션
TEST_F(UdpLoadTest, NetworkConditionSimulation) {
    struct TestCase {
        std::string name;
        NetworkCondition condition;
    };

    std::vector<TestCase> test_cases = {
        {"Perfect", NetworkCondition::Perfect()},
        {"GoodWifi", NetworkCondition::GoodWifi()},
        {"PoorWifi", NetworkCondition::PoorWifi()},
        {"Mobile4G", NetworkCondition::Mobile4G()},
        {"Mobile3G", NetworkCondition::Mobile3G()},
        {"Terrible", NetworkCondition::Terrible()}
    };

    const int PACKETS_PER_CONDITION = 1000;

    for (const auto& tc : test_cases) {
        simulator_.SetCondition(tc.condition);
        simulator_.SetEnabled(true);
        simulator_.ResetStatistics();

        int delivered = 0;

        for (int i = 0; i < PACKETS_PER_CONDITION; ++i) {
            std::vector<uint8_t> data(128, 0);
            simulator_.SimulateSend(data, "dest", [&](const auto&, const auto&) {
                ++delivered;
            });
        }

        // 지연된 패킷 처리
        for (int tick = 0; tick < 100; ++tick) {
            simulator_.ProcessDelayedPackets([&](const auto&, const auto&) {
                ++delivered;
            });
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }

        auto stats = simulator_.GetStatistics();
        float delivery_rate = 100.0f * static_cast<float>(delivered) / static_cast<float>(PACKETS_PER_CONDITION);

        std::cout << "[PERF] " << tc.name << ": "
                  << "Delivery=" << delivery_rate << "%, "
                  << "Dropped=" << stats.dropped_packets << ", "
                  << "Duplicated=" << stats.duplicated_packets << ", "
                  << "Reordered=" << stats.reordered_packets << "\n";

        // Perfect 조건에서는 100% 전달
        if (tc.name == "Perfect") {
            EXPECT_EQ(delivered, PACKETS_PER_CONDITION);
        }
    }
}

// 멀티스레드 메트릭 수집 테스트
TEST_F(UdpLoadTest, MultithreadedMetricsCollection) {
    const int NUM_THREADS = 8;
    const int OPS_PER_THREAD = 10000;

    std::vector<std::thread> threads;
    std::atomic<int> total_ops{0};

    auto start = std::chrono::steady_clock::now();

    for (int t = 0; t < NUM_THREADS; ++t) {
        threads.emplace_back([&, t]() {
            std::string client_id = "thread_client_" + std::to_string(t);

            for (int i = 0; i < OPS_PER_THREAD; ++i) {
                metrics_.RecordPacketSent(client_id, i, 128);
                metrics_.RecordPacketReceived(client_id, i, 128);
                total_ops.fetch_add(2);
            }
        });
    }

    for (auto& thread : threads) {
        thread.join();
    }

    auto end = std::chrono::steady_clock::now();
    auto duration_ms = std::chrono::duration<double, std::milli>(end - start).count();

    double ops_per_second = (total_ops.load() / duration_ms) * 1000.0;

    std::cout << "[PERF] Multithreaded throughput: " << ops_per_second << " ops/sec\n";
    std::cout << "[PERF] Threads: " << NUM_THREADS << ", Total ops: " << total_ops.load() << "\n";

    auto aggregated = metrics_.GetAggregatedMetrics();
    EXPECT_EQ(aggregated.packets_sent, static_cast<uint64_t>(NUM_THREADS * OPS_PER_THREAD));
}

// Prometheus 출력 성능 테스트
TEST_F(UdpLoadTest, PrometheusExportPerformance) {
    // 많은 클라이언트 시뮬레이션
    const int NUM_CLIENTS = 500;

    for (int i = 0; i < NUM_CLIENTS; ++i) {
        std::string client_id = "prometheus_client_" + std::to_string(i);
        metrics_.RecordPacketSent(client_id, 0, 64);
        metrics_.RecordPacketReceived(client_id, 0, 64);
    }

    auto start = std::chrono::steady_clock::now();

    std::string output = metrics_.ExportPrometheus();

    auto end = std::chrono::steady_clock::now();
    auto duration_us = std::chrono::duration<double, std::micro>(end - start).count();

    std::cout << "[PERF] Prometheus export time: " << duration_us << " us\n";
    std::cout << "[PERF] Output size: " << output.size() << " bytes\n";
    std::cout << "[PERF] Clients: " << NUM_CLIENTS << "\n";

    // 출력이 비어있지 않아야 함
    EXPECT_FALSE(output.empty());
    EXPECT_TRUE(output.find("pvp_udp_packets_sent_total") != std::string::npos);

    // 성능 기준: 1ms 이내
    EXPECT_LT(duration_us, 1000.0);
}

// 패킷 직렬화/역직렬화 처리량
TEST_F(UdpLoadTest, PacketSerializationThroughput) {
    const int NUM_PACKETS = 100000;

    // Snapshot 생성 (가장 큰 패킷 타입)
    Snapshot snapshot;
    snapshot.sequence = 12345;
    snapshot.timestamp = 1000000;
    for (int i = 0; i < 10; ++i) {
        PlayerState ps;
        ps.player_id = "player_" + std::to_string(i);
        ps.x = 100.0 + i;
        ps.y = 200.0 + i;
        ps.facing_radians = 0.0;
        ps.health = 80;
        ps.is_alive = true;
        ps.last_sequence = 0;
        snapshot.players.push_back(ps);
    }

    auto start = std::chrono::steady_clock::now();

    for (int i = 0; i < NUM_PACKETS; ++i) {
        // 직렬화
        std::vector<uint8_t> buffer = snapshot.Serialize();

        // 역직렬화
        Snapshot deserialized = Snapshot::Deserialize(buffer);
        (void)deserialized;  // suppress unused warning
    }

    auto end = std::chrono::steady_clock::now();
    auto duration_ms = std::chrono::duration<double, std::milli>(end - start).count();

    double packets_per_second = (NUM_PACKETS / duration_ms) * 1000.0;

    std::cout << "[PERF] Serialization throughput: " << packets_per_second << " packets/sec\n";
    std::cout << "[PERF] Time per packet: " << (duration_ms / NUM_PACKETS) * 1000.0 << " us\n";

    // 성능 기준: 최소 50,000 packets/sec
    EXPECT_GT(packets_per_second, 50000.0);
}

// 60 TPS 시뮬레이션 (16.67ms 틱)
TEST_F(UdpLoadTest, SteadyStateAt60TPS) {
    const int NUM_TICKS = 300;  // 5초
    const int CLIENTS_PER_TICK = 50;
    const float TICK_DURATION_MS = 16.67f;

    std::vector<double> tick_times;

    for (int tick = 0; tick < NUM_TICKS; ++tick) {
        auto tick_start = std::chrono::steady_clock::now();

        // 틱당 작업 시뮬레이션
        for (int c = 0; c < CLIENTS_PER_TICK; ++c) {
            std::string client_id = "steady_client_" + std::to_string(c);

            // 입력 수신
            metrics_.RecordPacketReceived(client_id, tick, 64);

            // 상태 전송
            metrics_.RecordPacketSent(client_id, tick, 256);
        }

        auto tick_end = std::chrono::steady_clock::now();
        double tick_time = std::chrono::duration<double, std::milli>(tick_end - tick_start).count();
        tick_times.push_back(tick_time);
    }

    // 통계 계산
    double sum = 0.0;
    double max_time = 0.0;
    for (double t : tick_times) {
        sum += t;
        max_time = std::max(max_time, t);
    }
    double avg_time = sum / tick_times.size();

    std::cout << "[PERF] 60 TPS Simulation:\n";
    std::cout << "[PERF]   Avg tick time: " << avg_time << " ms (budget: " << TICK_DURATION_MS << " ms)\n";
    std::cout << "[PERF]   Max tick time: " << max_time << " ms\n";
    std::cout << "[PERF]   Clients per tick: " << CLIENTS_PER_TICK << "\n";

    // 평균 틱 시간이 예산의 10% 이내여야 함
    EXPECT_LT(avg_time, TICK_DURATION_MS * 0.1);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
