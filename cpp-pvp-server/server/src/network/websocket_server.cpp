// [FILE]
// - 목적: WebSocket 기반 실시간 양방향 통신 서버
// - 주요 역할: 클라이언트 연결 관리, 입력 수신, 게임 상태 브로드캐스트
// - 관련 클론 가이드 단계: [CG-v1.0.0] 기본 게임 서버
// - 권장 읽는 순서: ClientSession 클래스 → WebSocketServer 클래스 → BroadcastState
//
// [LEARN] Boost.Beast는 HTTP/WebSocket 프로토콜 구현 라이브러리.
//         C에서 직접 HTTP 파싱 + 소켓 처리하던 것을 고수준 API로 제공.
//         비동기 I/O는 Boost.Asio 기반 (epoll/kqueue 추상화).

#include "pvpserver/network/websocket_server.h"

#include <atomic>
#include <boost/asio/post.hpp>
#include <chrono>
#include <iostream>
#include <limits>
#include <queue>
#include <sstream>
#include <utility>

namespace pvpserver {

namespace websocket = boost::beast::websocket;
using tcp = boost::asio::ip::tcp;

// [Order 1] ClientSession - 개별 클라이언트 연결을 관리하는 내부 클래스
// - WebSocket 연결 하나당 하나의 ClientSession 인스턴스
// - 입력 수신(ReadLoop) + 상태 전송(EnqueueState) 담당
// [LEARN] enable_shared_from_this는 자기 자신의 shared_ptr를 안전하게 얻는 패턴.
//         비동기 콜백에서 this 대신 shared_from_this()를 사용해야
//         객체가 콜백 실행 전에 소멸되는 것을 방지.
class WebSocketServer::ClientSession
    : public std::enable_shared_from_this<WebSocketServer::ClientSession> {
   public:
    ClientSession(WebSocketServer& server, tcp::socket socket)
        : server_(server), ws_(std::move(socket)) {}

    // WebSocket 핸드셰이크 시작 + 읽기 루프 진입
    // [LEARN] async_accept는 비동기 WebSocket 핸드셰이크.
    //         C에서 accept() 후 HTTP Upgrade 직접 처리하던 것을 한 줄로.
    void Start() {
        auto self = shared_from_this();
        ws_.set_option(websocket::stream_base::timeout::suggested(boost::beast::role_type::server));
        ws_.async_accept([self](boost::system::error_code ec) {
            if (ec) {
                std::cerr << "websocket accept error: " << ec.message() << std::endl;
                self->Stop();
                return;
            }
            self->ReadLoop();  // 핸드셰이크 성공 후 읽기 루프 시작
        });
    }

    // 연결 종료 처리
    // [LEARN] atomic<bool> closed_로 중복 종료 방지 (CAS 패턴)
    void Stop() {
        if (closed_.exchange(true)) {
            return;  // 이미 닫힌 경우 무시
        }
        boost::system::error_code ignored;
        ws_.close(websocket::close_code::normal, ignored);
        if (!player_id_.empty()) {
            server_.UnregisterClient(player_id_);
        }
    }

    // 게임 상태를 클라이언트에 전송 (큐잉)
    // [LEARN] boost::asio::post는 작업을 io_context 스레드에 위임.
    //         다른 스레드에서 호출해도 안전하게 처리됨.
    void EnqueueState(const PlayerState& state, std::uint64_t tick, double delta) {
        auto self = shared_from_this();
        boost::asio::post(ws_.get_executor(), [self, state, tick, delta]() {
            self->DoEnqueueState(state, tick, delta);
        });
    }

    void EnqueueDeath(const std::string& player_id, std::uint64_t tick) {
        auto self = shared_from_this();
        boost::asio::post(ws_.get_executor(),
                          [self, player_id, tick]() { self->DoEnqueueDeath(player_id, tick); });
    }

    const std::string& player_id() const { return player_id_; }

   private:
    // 상태 메시지 포맷팅 후 전송 큐에 추가
    void DoEnqueueState(const PlayerState& state, std::uint64_t tick, double delta) {
        // 프로토콜: "state <player_id> <x> <y> <facing> <tick> <delta> <hp> <alive> <shots> <hits> <deaths>"
        std::ostringstream oss;
        oss << "state " << state.player_id << ' ' << state.x << ' ' << state.y << ' '
            << state.facing_radians << ' ' << tick << ' ' << delta << ' ' << state.health << ' '
            << (state.is_alive ? 1 : 0) << ' ' << state.shots_fired << ' ' << state.hits_landed
            << ' ' << state.deaths;
        QueueMessage(oss.str());
    }

    // 사망 이벤트 메시지 포맷팅
    void DoEnqueueDeath(const std::string& player_id, std::uint64_t tick) {
        // 프로토콜: "death <player_id> <tick>"
        std::ostringstream oss;
        oss << "death " << player_id << ' ' << tick;
        QueueMessage(oss.str());
    }

    // 메시지 전송 큐 관리
    // [LEARN] 동시에 여러 async_write 호출 방지 (WebSocket 제약)
    //         writing_ 플래그로 한 번에 하나만 전송
    void QueueMessage(std::string message) {
        bool should_write = false;
        {
            std::lock_guard<std::mutex> lk(write_mutex_);
            write_queue_.push(std::move(message));
            if (!writing_) {
                writing_ = true;  // 쓰기 시작
                should_write = true;
            }
        }
        if (should_write) {
            DoWrite();
        }
    }

    // 비동기 쓰기 실행
    // [LEARN] async_write는 비동기 전송. 완료되면 OnWrite 콜백 호출.
    //         C의 write() + EAGAIN 처리를 자동화.
    void DoWrite() {
        std::string next;
        {
            std::lock_guard<std::mutex> lk(write_mutex_);
            if (write_queue_.empty()) {
                writing_ = false;
                return;
            }
            next = write_queue_.front();  // 복사 (전송 중 데이터 유지)
        }

        auto self = shared_from_this();
        ws_.async_write(boost::asio::buffer(next),
                        [self](boost::system::error_code ec, std::size_t /*bytes_transferred*/) {
                            self->OnWrite(ec);
                        });
    }

    void OnWrite(boost::system::error_code ec) {
        if (ec) {
            std::cerr << "websocket write error: " << ec.message() << std::endl;
            Stop();
            return;
        }
        {
            std::lock_guard<std::mutex> lk(write_mutex_);
            if (!write_queue_.empty()) {
                write_queue_.pop();
            }
            if (write_queue_.empty()) {
                writing_ = false;
                return;
            }
        }
        DoWrite();
    }

    // 비동기 읽기 루프
    // [LEARN] WebSocket 프레임 수신을 계속 대기.
    //         데이터 도착 시 OnRead 콜백 호출, 다시 ReadLoop 호출로 연속 수신.
    void ReadLoop() {
        auto self = shared_from_this();
        ws_.async_read(buffer_, [self](boost::system::error_code ec, std::size_t bytes_read) {
            if (ec) {
                if (ec != websocket::error::closed) {
                    std::cerr << "websocket read error: " << ec.message() << std::endl;
                }
                self->Stop();
                return;
            }
            self->OnRead(bytes_read);
        });
    }

    // 입력 프레임 수신 처리
    void OnRead(std::size_t /*bytes_read*/) {
        std::string data = boost::beast::buffers_to_string(buffer_.data());
        buffer_.consume(buffer_.size());  // 버퍼 비우기

        // 입력 프레임 파싱
        MovementInput input;
        std::string player_id;
        if (!ParseInputFrame(data, player_id, input)) {
            std::cerr << "invalid input frame: " << data << std::endl;
            ReadLoop();  // 잘못된 입력은 무시하고 계속
            return;
        }

        // 첫 입력 시 플레이어 ID 등록
        if (player_id_.empty()) {
            player_id_ = player_id;
            server_.RegisterClient(player_id_, shared_from_this());
        }

        // 입력을 게임 세션에 적용 (서버 권위)
        session_.ApplyInput(player_id_, input, loop_.TargetDelta());

        ReadLoop();  // 다음 입력 대기
    }

    // 입력 프레임 파싱
    // 프로토콜: "input <player_id> <seq> <up> <down> <left> <right> <mouse_x> <mouse_y> [fire]"
    bool ParseInputFrame(const std::string& data, std::string& player_id, MovementInput& input) {
        std::istringstream iss(data);
        std::string type;
        iss >> type;
        if (type != "input") {
            return false;
        }
        int up = 0, down = 0, left = 0, right = 0, fire = 0;
        iss >> player_id >> input.sequence >> up >> down >> left >> right >> input.mouse_x >>
            input.mouse_y;
        if (!iss) {
            return false;
        }
        if (!(iss >> fire)) {
            fire = 0;
            if (!iss.eof()) {
                iss.clear();
                iss.ignore(std::numeric_limits<std::streamsize>::max());
            } else {
                iss.clear();
            }
        }
        input.up = up != 0;
        input.down = down != 0;
        input.left = left != 0;
        input.right = right != 0;
        input.fire = fire != 0;
        return true;
    }

    WebSocketServer& server_;
    GameSession& session_{server_.session_};
    GameLoop& loop_{server_.loop_};
    websocket::stream<tcp::socket> ws_;
    boost::beast::flat_buffer buffer_;
    std::string player_id_;

    std::mutex write_mutex_;
    std::queue<std::string> write_queue_;
    bool writing_{false};
    std::atomic<bool> closed_{false};
};

// [Order 2] WebSocketServer - 메인 서버 클래스
// - 클라이언트 연결 수락 + 관리
// - GameLoop와 연동하여 틱마다 상태 브로드캐스트
// 클론 가이드 단계: [v1.0.0]
WebSocketServer::WebSocketServer(boost::asio::io_context& io_context, std::uint16_t port,
                                 GameSession& session, GameLoop& loop)
    : io_context_(io_context),
      acceptor_(io_context, tcp::endpoint(tcp::v4(), port)),
      session_(session),
      loop_(loop) {}

WebSocketServer::~WebSocketServer() { Stop(); }

// [Order 3] Start - 서버 시작
// - GameLoop에 틱 콜백 등록 (틱마다 BroadcastState 호출)
// - Accept 루프 시작
void WebSocketServer::Start() {
    if (running_.exchange(true)) {
        return;  // 이미 실행 중
    }
    auto self = shared_from_this();
    // GameLoop 틱마다 상태 브로드캐스트
    loop_.SetUpdateCallback([self](const TickInfo& tick) {
        boost::asio::post(self->io_context_,
                          [self, tick]() { self->BroadcastState(tick.tick, tick.delta_seconds); });
    });
    DoAccept();  // 클라이언트 연결 수락 시작
}

void WebSocketServer::Stop() {
    if (!running_) {
        return;
    }
    running_ = false;
    boost::system::error_code ec;
    acceptor_.close(ec);
    std::vector<std::shared_ptr<ClientSession>> alive;
    {
        std::lock_guard<std::mutex> lk(clients_mutex_);
        for (auto& kv : clients_) {
            if (auto client = kv.second.lock()) {
                alive.push_back(client);
            }
        }
        clients_.clear();
    }
    for (auto& client : alive) {
        client->Stop();
    }
}

std::string WebSocketServer::MetricsSnapshot() const {
    std::ostringstream oss;
    oss << "# TYPE websocket_connections_total gauge\n";
    oss << "websocket_connections_total " << connection_count_.load() << "\n";
    oss << session_.MetricsSnapshot();
    return oss.str();
}

std::uint16_t WebSocketServer::Port() const {
    boost::system::error_code ec;
    auto endpoint = acceptor_.local_endpoint(ec);
    if (ec) {
        return 0;
    }
    return endpoint.port();
}

// [Order 4] DoAccept - 클라이언트 연결 수락 루프
// [LEARN] async_accept는 비동기 accept. 연결 도착 시 콜백 호출.
//         C의 accept() 블로킹 호출과 달리, 이벤트 기반으로 동작.
void WebSocketServer::DoAccept() {
    acceptor_.async_accept(
        [self = shared_from_this()](boost::system::error_code ec, tcp::socket socket) {
            if (ec) {
                std::cerr << "accept error: " << ec.message() << std::endl;
                return;
            }
            // 새 클라이언트 세션 생성 및 시작
            auto client = std::make_shared<ClientSession>(*self, std::move(socket));
            client->Start();
            self->DoAccept();  // 다음 연결 대기 (재귀적)
        });
}

// [Order 5] BroadcastState - 틱마다 모든 클라이언트에 게임 상태 전송
// - GameLoop에서 60 TPS로 호출됨
// - 게임 세션 틱 처리 + 각 플레이어에게 자신의 상태 전송
void WebSocketServer::BroadcastState(std::uint64_t tick, double delta_seconds) {
    session_.Tick(tick, delta_seconds);  // 게임 로직 업데이트
    auto death_events = session_.ConsumeDeathEvents();
    std::vector<MatchResult> completed_matches;
    const bool has_callback = static_cast<bool>(match_completed_callback_);

    std::vector<std::shared_ptr<ClientSession>> alive;
    {
        std::lock_guard<std::mutex> lk(clients_mutex_);
        // 유효한 클라이언트만 수집 (weak_ptr → shared_ptr 변환)
        for (auto it = clients_.begin(); it != clients_.end();) {
            if (auto client = it->second.lock()) {
                alive.push_back(client);
                ++it;
            } else {
                it = clients_.erase(it);  // 이미 해제된 클라이언트 제거
            }
        }
    }

    // 각 클라이언트에게 자신의 상태 전송
    for (auto& client : alive) {
        try {
            auto state = session_.GetPlayer(client->player_id());
            client->EnqueueState(state, tick, delta_seconds);
        } catch (const std::exception& ex) {
            std::cerr << "state broadcast failed: " << ex.what() << std::endl;
        }
    }

    if (!death_events.empty()) {
        for (const auto& event : death_events) {
            if (event.type != CombatEventType::Death) {
                continue;
            }
            for (auto& client : alive) {
                client->EnqueueDeath(event.target_id, event.tick);
            }
            if (has_callback) {
                completed_matches.push_back(match_stats_collector_.Collect(
                    event, session_, std::chrono::system_clock::now()));
            }
        }
    }
    last_broadcast_tick_ = tick;

    if (has_callback) {
        for (const auto& match : completed_matches) {
            match_completed_callback_(match);
        }
    }
}

// [Order 6] RegisterClient - 새 클라이언트 등록
// - 기존 동일 ID 클라이언트가 있으면 끊고 교체
void WebSocketServer::RegisterClient(const std::string& player_id,
                                     std::shared_ptr<ClientSession> client) {
    std::shared_ptr<ClientSession> previous;
    {
        std::lock_guard<std::mutex> lk(clients_mutex_);
        auto it = clients_.find(player_id);
        if (it != clients_.end()) {
            previous = it->second.lock();  // 기존 연결 가져오기
        }
    }
    if (previous) {
        previous->Stop();  // 기존 연결 종료 (중복 로그인 방지)
    }
    {
        std::lock_guard<std::mutex> lk(clients_mutex_);
        clients_[player_id] = client;
        connection_count_.fetch_add(1, std::memory_order_relaxed);
    }
    session_.UpsertPlayer(player_id);
    if (on_join_) {
        on_join_(player_id);
    }
}

// [Order 7] UnregisterClient - 클라이언트 연결 해제 처리
void WebSocketServer::UnregisterClient(const std::string& player_id) {
    {
        std::lock_guard<std::mutex> lk(clients_mutex_);
        auto it = clients_.find(player_id);
        if (it != clients_.end()) {
            clients_.erase(it);
            // 연결 카운터 감소 (atomic)
            auto current = connection_count_.load(std::memory_order_relaxed);
            while (current != 0 && !connection_count_.compare_exchange_weak(
                                       current, current - 1, std::memory_order_relaxed)) {
            }
        }
    }
    session_.RemovePlayer(player_id);  // 게임 세션에서도 제거
    if (on_leave_) {
        on_leave_(player_id);
    }
}

void WebSocketServer::SetLifecycleHandlers(std::function<void(const std::string&)> on_join,
                                           std::function<void(const std::string&)> on_leave) {
    on_join_ = std::move(on_join);
    on_leave_ = std::move(on_leave);
}

void WebSocketServer::SetMatchCompletedCallback(std::function<void(const MatchResult&)> callback) {
    match_completed_callback_ = std::move(callback);
}

}  // namespace pvpserver

// [Reader Notes]
// ================================================================================
// 이 파일에서 처음 등장한 네트워크 프로그래밍 개념:
//
// 1. Boost.Beast WebSocket
//    - HTTP Upgrade를 통한 WebSocket 핸드셰이크 처리
//    - 비동기 프레임 송수신 (async_read, async_write)
//    - C에서 직접 HTTP 파싱 + 소켓 처리하던 것을 고수준 API로 제공
//
// 2. enable_shared_from_this 패턴
//    - 비동기 콜백에서 객체 수명 보장
//    - shared_from_this()로 자기 자신의 shared_ptr 획득
//    - C에서 콜백에 void* user_data 넘기던 것의 안전한 대안
//
// 3. 비동기 I/O 패턴 (Proactor)
//    - async_xxx() 호출 → 즉시 반환 → 완료 시 콜백 호출
//    - io_context.run()이 이벤트 루프 역할
//    - C의 epoll + 논블로킹 소켓 패턴과 유사하지만 더 추상화됨
//
// 4. weak_ptr + lock() 패턴
//    - clients_ 맵에 weak_ptr 저장 (순환 참조 방지)
//    - 사용 시 lock()으로 shared_ptr 획득, 실패하면 이미 해제됨
//
// 관련 설계 문서:
// - design/v1.0.0-game-server.md (WebSocket 서버 아키텍처)
//
// 이 파일을 이해한 다음, 이어서 보면 좋은 파일:
// - server/src/network/udp_game_server.cpp (UDP 버전)
// - server/src/network/snapshot_manager.cpp (상태 동기화)
// ================================================================================
