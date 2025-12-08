// [FILE]
// 목적: Prometheus 메트릭 노출용 HTTP 서버 - /metrics 엔드포인트 제공
// - 관련 클론 가이드 단계: [v1.3.0] Statistics & Ranking (Prometheus 연동)
// - 이 파일을 읽기 전에: design/v1.3.0-stats.md, websocket_server.cpp 참고
// - 학습 포인트:
//   [Order 1] Boost.Beast HTTP: 게임 서버 내장 HTTP 서버 (별도 웹서버 불필요)
//   [Order 2] enable_shared_from_this: 비동기 콜백에서 객체 수명 관리
//   [Order 3] RequestHandler 콜백: 라우팅 로직을 외부에서 주입 (의존성 역전)
// - [LEARN] Prometheus는 Pull 모델: 메트릭 서버가 주기적으로 /metrics를 폴링
// - [Reader Notes] 게임 서버가 9090 포트에서 메트릭 노출 → Grafana 대시보드 연동
// - 다음에 읽을 파일: profile_http_router.cpp (실제 라우팅 로직)

#include "pvpserver/network/metrics_http_server.h"

#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <iostream>
#include <memory>
#include <stdexcept>

namespace pvpserver {

namespace http = boost::beast::http;
using tcp = boost::asio::ip::tcp;

class MetricsHttpServer::Session : public std::enable_shared_from_this<MetricsHttpServer::Session> {
   public:
    Session(std::shared_ptr<MetricsHttpServer> server, tcp::socket socket)
        : server_(std::move(server)), socket_(std::move(socket)) {}

    void Start() { ReadRequest(); }

   private:
    void ReadRequest() {
        auto self = shared_from_this();
        http::async_read(socket_, buffer_, request_,
                         [self](boost::system::error_code ec, std::size_t /*bytes_transferred*/) {
                             if (ec) {
                                 if (ec != http::error::end_of_stream) {
                                     std::cerr << "metrics read error: " << ec.message()
                                               << std::endl;
                                 }
                                 self->Close();
                                 return;
                             }
                             self->HandleRequest();
                         });
    }

    void HandleRequest() {
        if (server_->handler_) {
            response_ = server_->handler_(request_);
        } else {
            response_ = {};
            response_.version(request_.version());
            response_.keep_alive(false);
            response_.result(http::status::not_found);
            response_.set(http::field::content_type, "text/plain");
            response_.body() = "Not Found";
            response_.prepare_payload();
        }

        auto self = shared_from_this();
        http::async_write(socket_, response_,
                          [self](boost::system::error_code ec, std::size_t /*bytes_transferred*/) {
                              if (ec) {
                                  std::cerr << "metrics write error: " << ec.message() << std::endl;
                              }
                              self->response_ = {};
                              self->Close();
                          });
    }

    void Close() {
        boost::system::error_code ec;
        socket_.shutdown(tcp::socket::shutdown_send, ec);
        socket_.close(ec);
    }

    std::shared_ptr<MetricsHttpServer> server_;
    tcp::socket socket_;
    boost::beast::flat_buffer buffer_;
    http::request<http::string_body> request_;
    http::response<http::string_body> response_;
};

MetricsHttpServer::MetricsHttpServer(boost::asio::io_context& io_context, std::uint16_t port,
                                     RequestHandler handler)
    : io_context_(io_context), acceptor_(io_context), handler_(std::move(handler)) {
    boost::system::error_code ec;
    tcp::endpoint endpoint(tcp::v4(), port);
    acceptor_.open(endpoint.protocol(), ec);
    if (ec) {
        throw std::runtime_error("metrics acceptor open failed: " + ec.message());
    }
    acceptor_.set_option(boost::asio::socket_base::reuse_address(true), ec);
    acceptor_.bind(endpoint, ec);
    if (ec) {
        throw std::runtime_error("metrics bind failed: " + ec.message());
    }
    acceptor_.listen(boost::asio::socket_base::max_listen_connections, ec);
    if (ec) {
        throw std::runtime_error("metrics listen failed: " + ec.message());
    }
}

MetricsHttpServer::~MetricsHttpServer() { Stop(); }

void MetricsHttpServer::Start() {
    if (running_.exchange(true)) {
        return;
    }
    DoAccept();
}

void MetricsHttpServer::Stop() {
    if (!running_) {
        return;
    }
    running_ = false;
    boost::system::error_code ec;
    acceptor_.close(ec);
}

std::uint16_t MetricsHttpServer::Port() const {
    boost::system::error_code ec;
    auto endpoint = acceptor_.local_endpoint(ec);
    if (ec) {
        return 0;
    }
    return endpoint.port();
}

void MetricsHttpServer::DoAccept() {
    acceptor_.async_accept(
        [self = shared_from_this()](boost::system::error_code ec, tcp::socket socket) {
            if (ec) {
                if (self->running_) {
                    std::cerr << "metrics accept error: " << ec.message() << std::endl;
                }
                return;
            }
            std::make_shared<Session>(self, std::move(socket))->Start();
            if (self->running_) {
                self->DoAccept();
            }
        });
}

}  // namespace pvpserver
