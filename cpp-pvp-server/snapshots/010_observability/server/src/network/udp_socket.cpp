#include "pvpserver/network/udp_socket.h"

#include <iostream>

namespace pvpserver {

UdpSocket::UdpSocket(boost::asio::io_context& io, std::uint16_t port)
    : socket_(io, boost::asio::ip::udp::endpoint(boost::asio::ip::udp::v4(), port)) {
}

UdpSocket::~UdpSocket() {
    StopReceive();
    if (socket_.is_open()) {
        boost::system::error_code ec;
        socket_.close(ec);
    }
}

void UdpSocket::StartReceive(ReceiveCallback callback) {
    receive_callback_ = std::move(callback);
    receiving_ = true;
    DoReceive();
}

void UdpSocket::StopReceive() {
    receiving_ = false;
}

void UdpSocket::DoReceive() {
    if (!receiving_) {
        return;
    }

    auto self = shared_from_this();
    socket_.async_receive_from(
        boost::asio::buffer(receive_buffer_),
        sender_endpoint_,
        [self](const boost::system::error_code& error, std::size_t bytes_transferred) {
            self->HandleReceive(error, bytes_transferred);
        }
    );
}

void UdpSocket::HandleReceive(
    const boost::system::error_code& error,
    std::size_t bytes_transferred
) {
    if (!receiving_) {
        return;
    }

    if (!error && bytes_transferred > 0) {
        // 통계 업데이트
        {
            std::lock_guard<std::mutex> lock(stats_mutex_);
            stats_.packets_received++;
            stats_.bytes_received += bytes_transferred;
        }

        // 콜백 호출
        if (receive_callback_) {
            std::vector<std::uint8_t> data(
                receive_buffer_.begin(),
                receive_buffer_.begin() + bytes_transferred
            );
            receive_callback_(data, sender_endpoint_);
        }
    } else if (error != boost::asio::error::operation_aborted) {
        std::cerr << "UDP receive error: " << error.message() << std::endl;
    }

    // 다음 수신 대기
    DoReceive();
}

void UdpSocket::SendTo(const std::vector<std::uint8_t>& data, const Endpoint& target) {
    if (data.empty() || data.size() > MAX_PACKET_SIZE) {
        return;
    }

    auto self = shared_from_this();
    auto buffer = std::make_shared<std::vector<std::uint8_t>>(data);

    socket_.async_send_to(
        boost::asio::buffer(*buffer),
        target,
        [self, buffer, size = data.size()](
            const boost::system::error_code& error,
            std::size_t /*bytes_transferred*/
        ) {
            if (!error) {
                std::lock_guard<std::mutex> lock(self->stats_mutex_);
                self->stats_.packets_sent++;
                self->stats_.bytes_sent += size;
            } else {
                std::cerr << "UDP send error: " << error.message() << std::endl;
            }
        }
    );
}

void UdpSocket::Broadcast(const std::vector<std::uint8_t>& data) {
    std::lock_guard<std::mutex> lock(clients_mutex_);
    for (const auto& client : clients_) {
        SendTo(data, client);
    }
}

void UdpSocket::RegisterClient(const Endpoint& client) {
    std::lock_guard<std::mutex> lock(clients_mutex_);
    clients_.insert(client);
}

void UdpSocket::UnregisterClient(const Endpoint& client) {
    std::lock_guard<std::mutex> lock(clients_mutex_);
    clients_.erase(client);
}

std::size_t UdpSocket::ClientCount() const {
    std::lock_guard<std::mutex> lock(clients_mutex_);
    return clients_.size();
}

std::uint16_t UdpSocket::LocalPort() const {
    return socket_.local_endpoint().port();
}

UdpSocket::Stats UdpSocket::GetStats() const {
    std::lock_guard<std::mutex> lock(stats_mutex_);
    return stats_;
}

}  // namespace pvpserver
