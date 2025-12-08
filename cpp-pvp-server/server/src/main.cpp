// [FILE]
// - 목적: PvP 게임 서버의 진입점 (main 함수)
// - 주요 역할: 모든 서버 컴포넌트를 초기화하고 연결하며, 이벤트 루프 실행
// - 관련 클론 가이드 단계: [v0.1.0] Bootstrap, [v1.0.0] 기본 서버
// - 권장 읽는 순서: main() → GameLoop/WebSocketServer 초기화 → 시그널 핸들러
//
// [LEARN] C++ 서버의 main.cpp는 C의 main()과 비슷하지만,
//         객체 생성/소멸 순서(RAII)와 비동기 이벤트 루프(io_context) 개념이 추가됨.
//         C에서 pthread + epoll을 직접 다루던 것을 Boost.Asio가 추상화해준다.

#include <boost/asio/io_context.hpp>
#include <boost/asio/signal_set.hpp>
#include <boost/asio/steady_timer.hpp>
#include <boost/beast/http.hpp>
#include <chrono>
#include <csignal>
#include <functional>
#include <iostream>
#include <memory>
#include <sstream>

#include "pvpserver/core/config.h"
#include "pvpserver/core/game_loop.h"
#include "pvpserver/game/game_session.h"
#include "pvpserver/matchmaking/match_queue.h"
#include "pvpserver/matchmaking/matchmaker.h"
#include "pvpserver/network/metrics_http_server.h"
#include "pvpserver/network/profile_http_router.h"
#include "pvpserver/network/websocket_server.h"
#include "pvpserver/stats/leaderboard_store.h"
#include "pvpserver/stats/player_profile_service.h"
#include "pvpserver/storage/postgres_storage.h"

// [Order 1] main 함수 - 서버의 진입점
// - 이 파일에서 가장 먼저 읽어야 할 부분
// - 전체 서버 아키텍처의 조립(wiring)이 여기서 일어남
// - 클론 가이드 단계: [v0.1.0], [v1.0.0]
int main() {
    using namespace pvpserver;

    // [LEARN] C++에서 const auto는 "타입 추론 + 불변"을 의미
    // C의 const struct config_t config = get_config(); 와 유사
    const auto config = GameConfig::FromEnv();
    std::cout << "PvP Server starting on port " << config.port() << std::endl;

    // [Order 2] 핵심 게임 컴포넌트 생성
    // - GameSession: 게임 상태 관리 (플레이어, 발사체, 충돌)
    // - GameLoop: 60 TPS 고정 틱레이트 루프
    // - PostgresStorage: 데이터베이스 연결
    // 클론 가이드 단계: [v1.0.0]
    GameSession session(config.tick_rate());
    GameLoop loop(config.tick_rate());
    PostgresStorage storage(config.database_dsn());
    if (!storage.Connect()) {
        std::cerr << "Failed to connect to Postgres at startup; continuing in degraded mode."
                  << std::endl;
    }

    // [Order 3] Boost.Asio io_context 및 서비스 객체들 생성
    // [LEARN] io_context는 C의 epoll/kqueue 이벤트 루프를 추상화한 것.
    //         모든 비동기 I/O 작업이 이 컨텍스트를 통해 스케줄링됨.
    //         C에서 while(1) { epoll_wait(...); handle_events(); } 패턴과 유사.
    // [LEARN] std::make_shared<T>()는 힙에 T를 할당하고 참조 카운팅 포인터 반환.
    //         C의 malloc + 수동 free 대신 자동 메모리 관리.
    boost::asio::io_context io_context;
    auto match_queue = std::make_shared<InMemoryMatchQueue>();
    auto matchmaker = std::make_shared<Matchmaker>(match_queue);
    auto leaderboard = std::make_shared<InMemoryLeaderboardStore>();
    auto profile_service = std::make_shared<PlayerProfileService>(leaderboard);
    auto server = std::make_shared<WebSocketServer>(io_context, config.port(), session, loop);

    // [Order 4] 플레이어 접속/해제 콜백 설정
    // [LEARN] 람다(Lambda)는 C의 함수 포인터 + 컨텍스트(클로저)를 합친 것.
    //         [&, matchmaker]는 외부 변수 캡처 방식을 지정:
    //         & = 나머지는 참조로, matchmaker = 값 복사로 캡처
    // 클론 가이드 단계: [v1.2.0] 매치메이킹
    server->SetLifecycleHandlers(
        [&, matchmaker](const std::string& player_id) {
            // 플레이어 접속 시: 매치메이킹 큐에 등록 + DB 이벤트 기록
            matchmaker->Enqueue(MatchRequest{player_id, 1200, std::chrono::steady_clock::now()});
            if (!storage.RecordSessionEvent(player_id, "start")) {
                std::cerr << "Failed to record session start for " << player_id << std::endl;
            }
        },
        [&, matchmaker](const std::string& player_id) {
            // 플레이어 해제 시: 매치메이킹 취소 + DB 이벤트 기록
            matchmaker->Cancel(player_id);
            if (!storage.RecordSessionEvent(player_id, "end")) {
                std::cerr << "Failed to record session end for " << player_id << std::endl;
            }
        });
    server->SetMatchCompletedCallback(
        [profile_service](const MatchResult& result) { profile_service->RecordMatch(result); });

    // [Order 5] Prometheus 메트릭 수집 및 HTTP 서버 설정
    // - 여러 컴포넌트의 메트릭을 하나로 합쳐서 /metrics 엔드포인트로 노출
    // 클론 가이드 단계: [v1.3.0] 통계 & 모니터링
    auto metrics_provider = [&, server, profile_service]() {
        std::ostringstream oss;
        oss << loop.PrometheusSnapshot();
        oss << server->MetricsSnapshot();
        oss << storage.MetricsSnapshot();
        oss << matchmaker->MetricsSnapshot();
        oss << profile_service->MetricsSnapshot();
        return oss.str();
    };
    auto router = std::make_shared<ProfileHttpRouter>(metrics_provider, profile_service);
    MetricsHttpServer::RequestHandler http_handler =
        [router](const boost::beast::http::request<boost::beast::http::string_body>& request) {
            return router->Handle(request);
        };
    auto metrics_server = std::make_shared<MetricsHttpServer>(io_context, config.metrics_port(),
                                                              std::move(http_handler));

    // [Order 6] 매치메이킹 주기 타이머 설정
    // [LEARN] steady_timer는 일정 간격으로 콜백을 실행하는 타이머.
    //         C의 timerfd + epoll 조합을 추상화한 것.
    //         200ms마다 매치메이킹 로직을 실행.
    // 클론 가이드 단계: [v1.2.0] 매치메이킹
    auto matchmaking_timer = std::make_shared<boost::asio::steady_timer>(io_context);
    std::function<void(const boost::system::error_code&)> matchmaking_tick;
    matchmaking_tick = [matchmaking_timer, matchmaker,
                        &matchmaking_tick](const boost::system::error_code& ec) {
        if (ec == boost::asio::error::operation_aborted) {
            return;
        }
        if (ec) {
            std::cerr << "matchmaking timer error: " << ec.message() << std::endl;
            return;
        }
        matchmaker->RunMatching(std::chrono::steady_clock::now());
        matchmaker->notification_channel().Drain();
        matchmaking_timer->expires_after(std::chrono::milliseconds(200));
        matchmaking_timer->async_wait(matchmaking_tick);
    };
    matchmaking_timer->expires_after(std::chrono::milliseconds(200));
    matchmaking_timer->async_wait(matchmaking_tick);

    // [Order 7] 시그널 핸들러 (graceful shutdown)
    // [LEARN] signal_set은 C의 sigaction/signal을 추상화.
    //         SIGINT(Ctrl+C)나 SIGTERM 수신 시 서버를 안전하게 종료.
    boost::asio::signal_set signals(io_context, SIGINT, SIGTERM);
    signals.async_wait([&](const boost::system::error_code& /*ec*/, int /*signal*/) {
        std::cout << "Signal received. Shutting down." << std::endl;
        server->Stop();
        metrics_server->Stop();
        loop.Stop();
        matchmaking_timer->cancel();
        io_context.stop();
    });

    server->Start();
    metrics_server->Start();
    std::cout << "Metrics endpoint listening on port " << metrics_server->Port() << std::endl;
    loop.Start();

    // [Order 8] 이벤트 루프 실행 (블로킹)
    // [LEARN] io_context.run()은 등록된 모든 비동기 작업이 완료될 때까지 블로킹.
    //         C의 while(1) { epoll_wait(); dispatch_events(); } 와 동일한 역할.
    //         이 호출이 반환되면 서버가 종료됨.
    io_context.run();

    loop.Stop();
    loop.Join();

    std::cout << "PvP Server stopped" << std::endl;
    return 0;
}

// [Reader Notes]
// ================================================================================
// 이 파일에서 처음 등장한 C++ 개념:
// 
// 1. Boost.Asio io_context
//    - C의 epoll/kqueue 이벤트 루프를 추상화한 클래스
//    - 모든 비동기 작업(네트워크, 타이머 등)이 이 컨텍스트를 통해 실행됨
//    - io_context.run()이 실제 이벤트 루프를 실행
//
// 2. std::shared_ptr<T>
//    - 참조 카운팅 기반 스마트 포인터 (C의 malloc/free 자동화)
//    - 마지막 참조가 사라지면 자동으로 메모리 해제
//    - std::make_shared<T>(args...)로 생성
//
// 3. 람다(Lambda) 표현식
//    - [캡처](매개변수) { 본문 } 형식
//    - [&]는 외부 변수를 참조로 캡처, [=]는 값으로 캡처
//    - C의 함수 포인터 + 컨텍스트(void* user_data)를 합친 것
//
// 4. RAII (Resource Acquisition Is Initialization)
//    - 객체 생성 시 리소스 획득, 소멸 시 자동 해제
//    - GameLoop, WebSocketServer 등이 소멸자에서 정리 작업 수행
//
// 관련 설계 문서:
// - design/v1.0.0-game-server.md (기본 서버 아키텍처)
// - design/v1.2.0-matchmaking.md (매치메이킹 시스템)
// - design/v1.3.0-stats.md (통계 & 모니터링)
//
// 이 파일을 이해한 다음, 이어서 보면 좋은 파일:
// - server/src/core/game_loop.cpp (60 TPS 게임 루프 구현)
// - server/src/network/websocket_server.cpp (WebSocket 통신)
// - server/src/game/game_session.cpp (게임 상태 관리)
// ================================================================================
