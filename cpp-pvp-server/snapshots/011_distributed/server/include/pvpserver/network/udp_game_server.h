#pragma once

#include <boost/asio/io_context.hpp>
#include <atomic>
#include <chrono>
#include <functional>
#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>

#include "pvpserver/core/game_loop.h"
#include "pvpserver/game/game_session.h"
#include "pvpserver/network/packet_types.h"
#include "pvpserver/network/udp_socket.h"

namespace pvpserver {

// Forward declaration
class SnapshotManager;

/**
 * @brief UDP 기반 게임 서버
 * 
 * 권위 서버 모델을 구현합니다:
 * - 모든 게임 로직은 서버에서 실행
 * - 클라이언트는 입력만 전송
 * - 서버가 게임 상태를 브로드캐스트
 */
class UdpGameServer : public std::enable_shared_from_this<UdpGameServer> {
   public:
    using Endpoint = boost::asio::ip::udp::endpoint;
    using MatchCompletedCallback = std::function<void(const MatchResult&)>;

    /**
     * @brief UDP 게임 서버 생성
     * @param io io_context 참조
     * @param port UDP 포트
     * @param session 게임 세션 참조
     * @param loop 게임 루프 참조
     */
    UdpGameServer(
        boost::asio::io_context& io,
        std::uint16_t port,
        GameSession& session,
        GameLoop& loop
    );

    ~UdpGameServer();

    /**
     * @brief 서버 시작
     */
    void Start();

    /**
     * @brief 서버 중지
     */
    void Stop();

    /**
     * @brief 서버 실행 중 여부
     */
    bool IsRunning() const { return running_; }

    /**
     * @brief 바인딩된 포트 반환
     */
    std::uint16_t Port() const;

    /**
     * @brief 연결된 클라이언트 수
     */
    std::size_t ClientCount() const;

    /**
     * @brief 메트릭 스냅샷 (Prometheus 형식)
     */
    std::string MetricsSnapshot() const;

    /**
     * @brief 생명주기 핸들러 설정
     */
    void SetLifecycleHandlers(
        std::function<void(const std::string&)> on_join,
        std::function<void(const std::string&)> on_leave
    );

    /**
     * @brief 매치 완료 콜백 설정
     */
    void SetMatchCompletedCallback(MatchCompletedCallback callback);

   private:
    // 클라이언트 정보
    struct ClientInfo {
        std::string player_id;
        Endpoint endpoint;
        std::uint32_t last_input_sequence{0};
        std::uint64_t last_heartbeat{0};
        std::uint64_t connect_time{0};
        std::uint32_t rtt_ms{0};
    };

    // 패킷 처리
    void OnPacketReceived(
        const std::vector<std::uint8_t>& data,
        const Endpoint& sender
    );

    void HandleConnect(const Endpoint& sender, const std::vector<std::uint8_t>& payload);
    void HandleDisconnect(const Endpoint& sender);
    void HandleHeartbeat(const Endpoint& sender, std::uint16_t sequence);
    void HandleInput(const Endpoint& sender, const std::vector<std::uint8_t>& payload);

    // 상태 브로드캐스트
    void BroadcastState(std::uint64_t tick, double delta_seconds);

    // 패킷 전송 헬퍼
    void SendPacket(
        const Endpoint& target,
        PacketType type,
        std::uint16_t sequence,
        const std::vector<std::uint8_t>& payload
    );

    void BroadcastPacket(
        PacketType type,
        std::uint16_t sequence,
        const std::vector<std::uint8_t>& payload
    );

    // 현재 시간 (밀리초)
    std::uint64_t CurrentTimeMs() const;

    // 클라이언트 검색
    ClientInfo* FindClient(const Endpoint& endpoint);
    ClientInfo* FindClientByPlayerId(const std::string& player_id);

    boost::asio::io_context& io_context_;
    std::shared_ptr<UdpSocket> socket_;
    std::atomic<bool> running_{false};

    GameSession& session_;
    GameLoop& loop_;

    // 클라이언트 관리
    mutable std::mutex clients_mutex_;
    std::unordered_map<std::string, ClientInfo> clients_;  // player_id → info
    std::unordered_map<std::size_t, std::string> endpoint_to_player_;  // endpoint hash → player_id

    // 콜백
    std::function<void(const std::string&)> on_join_;
    std::function<void(const std::string&)> on_leave_;
    MatchCompletedCallback match_completed_callback_;

    // 시퀀스 번호
    std::atomic<std::uint16_t> server_sequence_{0};
    std::atomic<std::uint32_t> current_tick_{0};

    // 스냅샷 관리 (v1.4.0-p3에서 구현)
    std::shared_ptr<SnapshotManager> snapshot_manager_;

    // 통계
    MatchStatsCollector match_stats_collector_;
};

}  // namespace pvpserver
