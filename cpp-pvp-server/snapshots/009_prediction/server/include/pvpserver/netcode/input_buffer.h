#pragma once

#include <cstdint>
#include <deque>
#include <optional>

#include "pvpserver/network/packet_types.h"

namespace pvpserver::netcode {

/**
 * @brief 타임스탬프가 붙은 입력
 */
struct TimestampedInput {
    std::uint32_t sequence;
    std::uint64_t client_timestamp;
    std::uint64_t server_receive_time;
    InputCommand command;
};

/**
 * @brief 입력 버퍼
 * 
 * 서버 측에서 입력을 버퍼링하여 네트워크 지터를 완화합니다.
 */
class InputBuffer {
   public:
    static constexpr std::size_t BUFFER_SIZE = 64;
    static constexpr int BUFFER_DELAY_MS = 50;  // 지터 완화용 딜레이

    /**
     * @brief 입력 추가
     */
    void Push(const TimestampedInput& input);

    /**
     * @brief 처리할 입력 가져오기 (시간순)
     */
    std::optional<InputCommand> Pop(std::uint64_t current_time);

    /**
     * @brief 특정 시퀀스의 입력 조회
     */
    std::optional<InputCommand> GetInput(std::uint32_t sequence) const;

    /**
     * @brief 시퀀스 범위의 입력들 조회
     */
    std::vector<InputCommand> GetInputRange(
        std::uint32_t start_seq,
        std::uint32_t end_seq
    ) const;

    /**
     * @brief 버퍼 크기
     */
    std::size_t Size() const { return buffer_.size(); }

    /**
     * @brief 버퍼가 비었는지
     */
    bool IsEmpty() const { return buffer_.empty(); }

    /**
     * @brief 버퍼 지연 설정 (밀리초)
     */
    void SetBufferDelay(int delay_ms) { buffer_delay_ms_ = delay_ms; }

    /**
     * @brief 버퍼 상태 통계
     */
    struct Stats {
        std::uint64_t inputs_received{0};
        std::uint64_t inputs_processed{0};
        std::uint64_t inputs_dropped{0};  // 오래된 입력
    };
    Stats GetStats() const { return stats_; }

   private:
    std::deque<TimestampedInput> buffer_;
    std::uint32_t last_processed_sequence_{0};
    int buffer_delay_ms_{BUFFER_DELAY_MS};
    Stats stats_;
};

}  // namespace pvpserver::netcode
