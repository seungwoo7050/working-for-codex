#include "pvpserver/network/udp_game_server.h"

#include <chrono>
#include <cmath>
#include <iostream>

#include "pvpserver/core/game_loop.h"

namespace pvpserver {

namespace {

// 엔드포인트를 해시 가능한 키로 변환
std::size_t EndpointHash(const boost::asio::ip::udp::endpoint& ep) {
    std::size_t h1 = std::hash<std::string>{}(ep.address().to_string());
    std::size_t h2 = std::hash<std::uint16_t>{}(ep.port());
    return h1 ^ (h2 << 1);
}

}  // namespace

UdpGameServer::UdpGameServer(
    boost::asio::io_context& io,
    std::uint16_t port,
    GameSession& session,
    GameLoop& loop
)
    : io_context_(io),
      session_(session),
      loop_(loop) {
    socket_ = std::make_shared<UdpSocket>(io, port);
}

UdpGameServer::~UdpGameServer() {
    Stop();
}

void UdpGameServer::Start() {
    if (running_) {
        return;
    }

    running_ = true;

    // 패킷 수신 시작
    auto self = shared_from_this();
    socket_->StartReceive([self](
        const std::vector<std::uint8_t>& data,
        const UdpSocket::Endpoint& sender
    ) {
        self->OnPacketReceived(data, sender);
    });

    // 게임 루프에 브로드캐스트 콜백 등록
    loop_.SetUpdateCallback([self](const TickInfo& info) {
        self->BroadcastState(info.tick, info.delta_seconds);
    });

    std::cout << "UDP Game Server started on port " << Port() << std::endl;
}

void UdpGameServer::Stop() {
    if (!running_) {
        return;
    }

    running_ = false;
    socket_->StopReceive();
    loop_.SetUpdateCallback(nullptr);

    std::cout << "UDP Game Server stopped" << std::endl;
}

std::uint16_t UdpGameServer::Port() const {
    return socket_->LocalPort();
}

std::size_t UdpGameServer::ClientCount() const {
    std::lock_guard<std::mutex> lock(clients_mutex_);
    return clients_.size();
}

std::string UdpGameServer::MetricsSnapshot() const {
    auto stats = socket_->GetStats();
    std::string result;
    result += "# HELP pvp_udp_packets_total Total UDP packets\n";
    result += "# TYPE pvp_udp_packets_total counter\n";
    result += "pvp_udp_packets_total{direction=\"sent\"} " + std::to_string(stats.packets_sent) + "\n";
    result += "pvp_udp_packets_total{direction=\"received\"} " + std::to_string(stats.packets_received) + "\n";
    result += "# HELP pvp_udp_bytes_total Total UDP bytes\n";
    result += "# TYPE pvp_udp_bytes_total counter\n";
    result += "pvp_udp_bytes_total{direction=\"sent\"} " + std::to_string(stats.bytes_sent) + "\n";
    result += "pvp_udp_bytes_total{direction=\"received\"} " + std::to_string(stats.bytes_received) + "\n";
    result += "# HELP pvp_udp_clients_connected Connected clients\n";
    result += "# TYPE pvp_udp_clients_connected gauge\n";
    result += "pvp_udp_clients_connected " + std::to_string(ClientCount()) + "\n";
    return result;
}

void UdpGameServer::SetLifecycleHandlers(
    std::function<void(const std::string&)> on_join,
    std::function<void(const std::string&)> on_leave
) {
    on_join_ = std::move(on_join);
    on_leave_ = std::move(on_leave);
}

void UdpGameServer::SetMatchCompletedCallback(MatchCompletedCallback callback) {
    match_completed_callback_ = std::move(callback);
}

void UdpGameServer::OnPacketReceived(
    const std::vector<std::uint8_t>& data,
    const Endpoint& sender
) {
    if (data.size() < PacketHeader::SIZE) {
        return;  // 헤더 부족
    }

    try {
        auto header = PacketHeader::Deserialize(data);
        std::vector<std::uint8_t> payload(
            data.begin() + PacketHeader::SIZE,
            data.end()
        );

        switch (header.type) {
            case PacketType::CONNECT:
                HandleConnect(sender, payload);
                break;
            case PacketType::DISCONNECT:
                HandleDisconnect(sender);
                break;
            case PacketType::HEARTBEAT:
                HandleHeartbeat(sender, header.sequence);
                break;
            case PacketType::INPUT:
                HandleInput(sender, payload);
                break;
            default:
                // 알 수 없는 패킷 타입
                break;
        }
    } catch (const std::exception& e) {
        std::cerr << "Packet parse error: " << e.what() << std::endl;
    }
}

void UdpGameServer::HandleConnect(
    const Endpoint& sender,
    const std::vector<std::uint8_t>& payload
) {
    try {
        auto connect = ConnectPacket::Deserialize(payload);
        
        std::lock_guard<std::mutex> lock(clients_mutex_);
        
        // 이미 연결된 플레이어인지 확인
        if (clients_.find(connect.player_id) != clients_.end()) {
            // 재연결 처리: 엔드포인트 업데이트
            auto& info = clients_[connect.player_id];
            auto old_hash = EndpointHash(info.endpoint);
            endpoint_to_player_.erase(old_hash);
            
            info.endpoint = sender;
            info.last_heartbeat = CurrentTimeMs();
            endpoint_to_player_[EndpointHash(sender)] = connect.player_id;
        } else {
            // 새 연결
            ClientInfo info;
            info.player_id = connect.player_id;
            info.endpoint = sender;
            info.connect_time = CurrentTimeMs();
            info.last_heartbeat = CurrentTimeMs();
            
            clients_[connect.player_id] = info;
            endpoint_to_player_[EndpointHash(sender)] = connect.player_id;
            
            socket_->RegisterClient(sender);
            session_.UpsertPlayer(connect.player_id);
            
            if (on_join_) {
                on_join_(connect.player_id);
            }
        }
        
        // ConnectAck 전송
        ConnectAckPacket ack;
        ack.assigned_id = connect.player_id;
        ack.server_tick = current_tick_;
        ack.tick_rate = 60;
        
        SendPacket(sender, PacketType::CONNECT_ACK, server_sequence_++, ack.Serialize());
        
        std::cout << "Client connected: " << connect.player_id 
                  << " from " << sender.address().to_string() 
                  << ":" << sender.port() << std::endl;
                  
    } catch (const std::exception& e) {
        std::cerr << "HandleConnect error: " << e.what() << std::endl;
    }
}

void UdpGameServer::HandleDisconnect(const Endpoint& sender) {
    std::lock_guard<std::mutex> lock(clients_mutex_);
    
    auto hash = EndpointHash(sender);
    auto it = endpoint_to_player_.find(hash);
    if (it == endpoint_to_player_.end()) {
        return;
    }
    
    std::string player_id = it->second;
    endpoint_to_player_.erase(it);
    clients_.erase(player_id);
    
    socket_->UnregisterClient(sender);
    session_.RemovePlayer(player_id);
    
    if (on_leave_) {
        on_leave_(player_id);
    }
    
    std::cout << "Client disconnected: " << player_id << std::endl;
}

void UdpGameServer::HandleHeartbeat(const Endpoint& sender, std::uint16_t sequence) {
    std::lock_guard<std::mutex> lock(clients_mutex_);
    
    auto* client = FindClient(sender);
    if (!client) {
        return;
    }
    
    auto now = CurrentTimeMs();
    client->last_heartbeat = now;
    
    // HeartbeatAck 전송
    SendPacket(sender, PacketType::HEARTBEAT_ACK, sequence, {});
}

void UdpGameServer::HandleInput(
    const Endpoint& sender,
    const std::vector<std::uint8_t>& payload
) {
    try {
        auto input_cmd = InputCommand::Deserialize(payload);
        
        std::string player_id;
        {
            std::lock_guard<std::mutex> lock(clients_mutex_);
            auto* client = FindClient(sender);
            if (!client) {
                return;
            }
            
            // 중복/오래된 입력 무시
            if (input_cmd.sequence <= client->last_input_sequence) {
                return;
            }
            
            client->last_input_sequence = input_cmd.sequence;
            player_id = client->player_id;
        }
        
        // MovementInput으로 변환
        MovementInput movement;
        movement.sequence = input_cmd.sequence;
        movement.up = input_cmd.move_y > 0.5f;
        movement.down = input_cmd.move_y < -0.5f;
        movement.left = input_cmd.move_x < -0.5f;
        movement.right = input_cmd.move_x > 0.5f;
        movement.mouse_x = std::cos(input_cmd.aim_radians);
        movement.mouse_y = std::sin(input_cmd.aim_radians);
        movement.fire = input_cmd.fire;
        
        // 게임 세션에 입력 적용
        session_.ApplyInput(player_id, movement, 1.0 / 60.0);
        
        // InputAck 전송 (선택적)
        // SendPacket(sender, PacketType::INPUT_ACK, input_cmd.sequence, {});
        
    } catch (const std::exception& e) {
        std::cerr << "HandleInput error: " << e.what() << std::endl;
    }
}

void UdpGameServer::BroadcastState(std::uint64_t tick, double /*delta_seconds*/) {
    current_tick_ = static_cast<std::uint32_t>(tick);
    
    // 플레이어 상태 스냅샷
    auto players = session_.Snapshot();
    
    // 페이로드 직렬화
    std::vector<std::uint8_t> payload;
    
    // 틱 번호 (4 바이트)
    std::uint32_t tick32 = current_tick_;
    payload.push_back((tick32 >> 24) & 0xFF);
    payload.push_back((tick32 >> 16) & 0xFF);
    payload.push_back((tick32 >> 8) & 0xFF);
    payload.push_back(tick32 & 0xFF);
    
    // 플레이어 수 (1 바이트)
    payload.push_back(static_cast<std::uint8_t>(std::min(players.size(), static_cast<std::size_t>(255))));
    
    // 각 플레이어 상태
    for (const auto& player : players) {
        PlayerSnapshot snapshot;
        snapshot.player_id = player.player_id;
        snapshot.x = static_cast<float>(player.x);
        snapshot.y = static_cast<float>(player.y);
        snapshot.facing_radians = static_cast<float>(player.facing_radians);
        snapshot.health = player.health;
        snapshot.is_alive = player.is_alive;
        snapshot.last_input_sequence = static_cast<std::uint32_t>(player.last_sequence);
        
        auto serialized = snapshot.Serialize();
        payload.insert(payload.end(), serialized.begin(), serialized.end());
    }
    
    // 브로드캐스트
    BroadcastPacket(PacketType::STATE_FULL, server_sequence_++, payload);
    
    // 사망 이벤트 처리
    auto deaths = session_.ConsumeDeathEvents();
    for (const auto& death : deaths) {
        GameEvent event;
        event.type = GameEventType::PLAYER_DEATH;
        event.timestamp = CurrentTimeMs();
        event.data = death.victim_id;
        
        BroadcastPacket(PacketType::EVENT, server_sequence_++, event.Serialize());
    }
}

void UdpGameServer::SendPacket(
    const Endpoint& target,
    PacketType type,
    std::uint16_t sequence,
    const std::vector<std::uint8_t>& payload
) {
    PacketHeader header;
    header.type = type;
    header.sequence = sequence;
    header.length = static_cast<std::uint8_t>(std::min(payload.size(), static_cast<std::size_t>(255)));
    
    auto header_bytes = header.Serialize();
    std::vector<std::uint8_t> packet;
    packet.reserve(header_bytes.size() + payload.size());
    packet.insert(packet.end(), header_bytes.begin(), header_bytes.end());
    packet.insert(packet.end(), payload.begin(), payload.end());
    
    socket_->SendTo(packet, target);
}

void UdpGameServer::BroadcastPacket(
    PacketType type,
    std::uint16_t sequence,
    const std::vector<std::uint8_t>& payload
) {
    PacketHeader header;
    header.type = type;
    header.sequence = sequence;
    header.length = static_cast<std::uint8_t>(std::min(payload.size(), static_cast<std::size_t>(255)));
    
    auto header_bytes = header.Serialize();
    std::vector<std::uint8_t> packet;
    packet.reserve(header_bytes.size() + payload.size());
    packet.insert(packet.end(), header_bytes.begin(), header_bytes.end());
    packet.insert(packet.end(), payload.begin(), payload.end());
    
    socket_->Broadcast(packet);
}

std::uint64_t UdpGameServer::CurrentTimeMs() const {
    return std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now().time_since_epoch()
    ).count();
}

UdpGameServer::ClientInfo* UdpGameServer::FindClient(const Endpoint& endpoint) {
    auto hash = EndpointHash(endpoint);
    auto it = endpoint_to_player_.find(hash);
    if (it == endpoint_to_player_.end()) {
        return nullptr;
    }
    auto client_it = clients_.find(it->second);
    if (client_it == clients_.end()) {
        return nullptr;
    }
    return &client_it->second;
}

UdpGameServer::ClientInfo* UdpGameServer::FindClientByPlayerId(const std::string& player_id) {
    auto it = clients_.find(player_id);
    if (it == clients_.end()) {
        return nullptr;
    }
    return &it->second;
}

}  // namespace pvpserver
