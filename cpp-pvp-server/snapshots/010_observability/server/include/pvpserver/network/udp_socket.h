#pragma once

#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/udp.hpp>
#include <array>
#include <atomic>
#include <functional>
#include <memory>
#include <mutex>
#include <set>
#include <vector>

namespace pvpserver {

/**
 * @brief UDP 소켓 래퍼 클래스
 * 
 * Boost.Asio 기반 비동기 UDP 소켓을 추상화합니다.
 * 패킷 손실/재정렬은 상위 레이어에서 처리합니다.
 */
class UdpSocket : public std::enable_shared_from_this<UdpSocket> {
   public:
    using Endpoint = boost::asio::ip::udp::endpoint;
    using ReceiveCallback = std::function<void(
        const std::vector<std::uint8_t>& data,
        const Endpoint& sender
    )>;

    static constexpr std::size_t MTU_SIZE = 1500;
    static constexpr std::size_t MAX_PACKET_SIZE = 1400;  // MTU - IP/UDP 헤더 여유

    /**
     * @brief UDP 소켓 생성
     * @param io io_context 참조
     * @param port 바인딩할 포트 (0이면 임의 포트)
     */
    explicit UdpSocket(boost::asio::io_context& io, std::uint16_t port = 0);
    ~UdpSocket();

    // 복사 금지
    UdpSocket(const UdpSocket&) = delete;
    UdpSocket& operator=(const UdpSocket&) = delete;

    /**
     * @brief 비동기 수신 시작
     * @param callback 패킷 수신 시 호출될 콜백
     */
    void StartReceive(ReceiveCallback callback);

    /**
     * @brief 수신 중지
     */
    void StopReceive();

    /**
     * @brief 특정 엔드포인트로 데이터 전송
     * @param data 전송할 데이터
     * @param target 대상 엔드포인트
     */
    void SendTo(const std::vector<std::uint8_t>& data, const Endpoint& target);

    /**
     * @brief 등록된 모든 클라이언트에게 브로드캐스트
     * @param data 전송할 데이터
     */
    void Broadcast(const std::vector<std::uint8_t>& data);

    /**
     * @brief 클라이언트 엔드포인트 등록
     * @param client 등록할 엔드포인트
     */
    void RegisterClient(const Endpoint& client);

    /**
     * @brief 클라이언트 엔드포인트 해제
     * @param client 해제할 엔드포인트
     */
    void UnregisterClient(const Endpoint& client);

    /**
     * @brief 등록된 클라이언트 수 반환
     */
    std::size_t ClientCount() const;

    /**
     * @brief 바인딩된 로컬 포트 반환
     */
    std::uint16_t LocalPort() const;

    /**
     * @brief 전송 통계
     */
    struct Stats {
        std::uint64_t packets_sent{0};
        std::uint64_t packets_received{0};
        std::uint64_t bytes_sent{0};
        std::uint64_t bytes_received{0};
    };
    Stats GetStats() const;

   private:
    void DoReceive();
    void HandleReceive(
        const boost::system::error_code& error,
        std::size_t bytes_transferred
    );

    boost::asio::ip::udp::socket socket_;
    Endpoint sender_endpoint_;
    std::array<std::uint8_t, MTU_SIZE> receive_buffer_;

    ReceiveCallback receive_callback_;
    std::atomic<bool> receiving_{false};

    mutable std::mutex clients_mutex_;
    std::set<Endpoint> clients_;

    // 통계
    mutable std::mutex stats_mutex_;
    Stats stats_;
};

}  // namespace pvpserver
