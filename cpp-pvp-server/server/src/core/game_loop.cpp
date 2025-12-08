// [FILE]
// - 목적: 고정 틱레이트 게임 루프 구현 (기본 60 TPS)
// - 주요 역할: 일정한 시간 간격으로 게임 상태 업데이트를 트리거
// - 관련 클론 가이드 단계: [v1.0.0] 기본 게임 서버
// - 권장 읽는 순서: GameLoop 생성자 → Start() → Run() → Stop()
//
// [LEARN] 게임 서버의 핵심 개념 중 하나가 "고정 틱레이트"다.
//         물리/충돌 같은 시뮬레이션은 delta_time이 일정해야 결과가 예측 가능하다.
//         이 파일은 C의 gettimeofday + nanosleep 조합을 C++ 스타일로 구현한 것.

#include "pvpserver/core/game_loop.h"

#include <cmath>
#include <iostream>
#include <sstream>

namespace pvpserver {

// [Order 1] 생성자 - 틱레이트 설정
// - tick_rate: 초당 틱 수 (예: 60.0 = 60 TPS)
// - target_delta_: 틱당 목표 시간 (1/60 ≈ 16.67ms)
// 클론 가이드 단계: [v1.0.0]
GameLoop::GameLoop(double tick_rate)
    : tick_rate_(tick_rate), target_delta_(std::chrono::duration<double>(1.0 / tick_rate)) {}

// [Order 2] 소멸자 - RAII 패턴으로 리소스 정리
// [LEARN] C++의 RAII: 객체 소멸 시 자동으로 Stop/Join 호출.
//         C에서 pthread_join을 main 끝에 수동으로 넣던 것과 달리,
//         소멸자에서 자동 처리되므로 리소스 누수 방지.
GameLoop::~GameLoop() {
    Stop();
    Join();
}

// [Order 3] Start() - 게임 루프 스레드 시작
// - compare_exchange_strong: 원자적으로 running_ 상태 확인 및 설정
// - 별도 스레드에서 Run() 실행
// [LEARN] std::thread는 C의 pthread_create를 래핑한 것.
//         [this]() { Run(); }는 멤버 함수를 스레드에서 실행하는 람다.
void GameLoop::Start() {
    bool expected = false;
    if (!running_.compare_exchange_strong(expected, true)) {
        return;  // 이미 실행 중이면 무시
    }
    {
        std::lock_guard<std::mutex> lk(metrics_mutex_);
        last_durations_.clear();
        tick_counter_ = 0;
    }
    stop_requested_ = false;
    thread_ = std::thread([this]() { Run(); });
}

// [Order 4] Stop() - 루프 종료 요청
// - stop_requested_ 플래그 설정 후 조건 변수로 알림
// [LEARN] condition_variable은 C의 pthread_cond_signal과 같은 역할.
//         대기 중인 스레드를 깨워서 종료하게 함.
void GameLoop::Stop() {
    if (!running_) {
        return;
    }
    {
        std::lock_guard<std::mutex> lk(mutex_);
        stop_requested_ = true;
    }
    stop_cv_.notify_all();  // 대기 중인 스레드 깨우기
}

// [Order 5] Join() - 스레드 종료 대기
// [LEARN] std::thread::join()은 pthread_join과 동일.
//         스레드가 완전히 종료될 때까지 블로킹.
void GameLoop::Join() {
    if (thread_.joinable()) {
        thread_.join();
    }
}

void GameLoop::SetUpdateCallback(std::function<void(const TickInfo&)> callback) {
    std::lock_guard<std::mutex> lk(mutex_);
    callback_ = std::move(callback);
}

double GameLoop::TargetDelta() const noexcept { return target_delta_.count(); }

double GameLoop::CurrentTickRate() const {
    std::lock_guard<std::mutex> lk(metrics_mutex_);
    if (last_durations_.empty()) {
        return tick_rate_;
    }
    const double latest = last_durations_.back();
    if (latest == 0.0) {
        return tick_rate_;
    }
    return 1.0 / latest;
}

std::vector<double> GameLoop::LastDurations() const {
    std::lock_guard<std::mutex> lk(metrics_mutex_);
    return last_durations_;
}

std::string GameLoop::PrometheusSnapshot() const {
    std::ostringstream oss;
    oss << "# TYPE game_tick_rate gauge\n";
    oss << "game_tick_rate " << CurrentTickRate() << "\n";
    double last_duration = TargetDelta();
    {
        std::lock_guard<std::mutex> lk(metrics_mutex_);
        if (!last_durations_.empty()) {
            last_duration = last_durations_.back();
        }
    }
    oss << "# TYPE game_tick_duration_seconds gauge\n";
    oss << "game_tick_duration_seconds " << last_duration << "\n";
    return oss.str();
}

// [Order 6] Run() - 핵심 루프 로직 (스레드에서 실행)
// - 고정 틱레이트를 유지하면서 콜백 함수 호출
// - delta_seconds: 이전 프레임으로부터 경과 시간
// - 클론 가이드 단계: [v1.0.0]
//
// [LEARN] 게임 루프의 핵심 패턴:
//         1. 현재 시간 측정
//         2. 델타 타임 계산
//         3. 게임 로직 업데이트(콜백 호출)
//         4. 다음 프레임까지 sleep
//         이 과정을 반복하여 일정한 TPS 유지
void GameLoop::Run() {
    auto previous = std::chrono::steady_clock::now();
    auto next_frame = previous + target_delta_;
    while (true) {
        // 종료 요청 확인
        {
            std::unique_lock<std::mutex> lk(mutex_);
            if (stop_requested_) {
                running_ = false;
                break;
            }
        }

        // 델타 타임 계산
        const auto frame_start = std::chrono::steady_clock::now();
        const auto elapsed = frame_start - previous;

        const double delta_seconds = std::chrono::duration<double>(elapsed).count();
        const TickInfo info{tick_counter_, delta_seconds, frame_start};

        // 등록된 콜백 호출 (게임 상태 업데이트)
        {
            std::lock_guard<std::mutex> lk(mutex_);
            if (callback_) {
                callback_(info);
            }
        }

        previous = frame_start;
        next_frame += target_delta_;

        {
            std::lock_guard<std::mutex> lk(metrics_mutex_);
            last_durations_.push_back(delta_seconds);
            if (last_durations_.size() > 240) {
                last_durations_.erase(last_durations_.begin());
            }
            ++tick_counter_;
        }

        // 다음 프레임까지 대기 (고정 틱레이트 유지)
        // [LEARN] condition_variable::wait_for는 C의 nanosleep과 유사하지만,
        //         stop_requested_ 시그널을 받으면 즉시 깨어남.
        const auto now = std::chrono::steady_clock::now();
        const auto sleep_duration = next_frame - now;
        if (sleep_duration.count() > 0) {
            std::unique_lock<std::mutex> lk(mutex_);
            stop_cv_.wait_for(lk, sleep_duration, [this]() { return stop_requested_; });
        } else {
            // 프레임 처리가 너무 오래 걸린 경우, 시간 기준 재설정
            next_frame = std::chrono::steady_clock::now();
        }
    }
}

}  // namespace pvpserver

// [Reader Notes]
// ================================================================================
// 이 파일에서 처음 등장한 C++ 개념:
//
// 1. std::thread
//    - pthread_create/pthread_join을 래핑한 클래스
//    - 생성자에 실행할 함수(또는 람다)를 전달
//    - joinable()로 합류 가능 여부 확인, join()으로 대기
//
// 2. std::mutex + std::lock_guard
//    - pthread_mutex_lock/unlock을 RAII로 감싼 것
//    - lock_guard 생성 시 자동 lock, 소멸 시 자동 unlock
//    - 예외가 발생해도 unlock이 보장됨
//
// 3. std::condition_variable
//    - pthread_cond_wait/signal과 동일
//    - wait_for(): 타임아웃 + 조건 확인
//    - notify_all(): 대기 중인 모든 스레드 깨우기
//
// 4. std::atomic<bool>
//    - lock-free 원자적 변수 (C11 _Atomic과 동일)
//    - compare_exchange_strong: CAS(Compare-And-Swap) 연산
//
// 5. std::chrono
//    - 고해상도 시간 측정 라이브러리
//    - steady_clock: 단조 증가 시계 (시스템 시간 변경에 영향 없음)
//    - duration: 시간 간격, time_point: 특정 시점
//
// 관련 설계 문서:
// - design/v1.0.0-game-server.md (60 TPS 고정 틱레이트)
//
// 이 파일을 이해한 다음, 이어서 보면 좋은 파일:
// - server/src/game/game_session.cpp (틱마다 호출되는 게임 로직)
// - server/src/network/websocket_server.cpp (틱 결과 브로드캐스트)
// ================================================================================
