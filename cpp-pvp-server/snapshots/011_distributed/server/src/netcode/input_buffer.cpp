#include "pvpserver/netcode/input_buffer.h"

#include <algorithm>

namespace pvpserver::netcode {

void InputBuffer::Push(const TimestampedInput& input) {
    stats_.inputs_received++;
    
    // 오래된 입력 무시
    if (input.sequence <= last_processed_sequence_) {
        stats_.inputs_dropped++;
        return;
    }
    
    // 시퀀스 순서대로 삽입
    auto it = std::lower_bound(
        buffer_.begin(), buffer_.end(), input,
        [](const TimestampedInput& a, const TimestampedInput& b) {
            return a.sequence < b.sequence;
        }
    );
    
    // 중복 체크
    if (it != buffer_.end() && it->sequence == input.sequence) {
        return;  // 이미 존재
    }
    
    buffer_.insert(it, input);
    
    // 버퍼 크기 제한
    while (buffer_.size() > BUFFER_SIZE) {
        buffer_.pop_front();
        stats_.inputs_dropped++;
    }
}

std::optional<InputCommand> InputBuffer::Pop(std::uint64_t current_time) {
    if (buffer_.empty()) {
        return std::nullopt;
    }
    
    // 버퍼 딜레이 적용
    const auto& front = buffer_.front();
    std::uint64_t ready_time = front.server_receive_time + buffer_delay_ms_;
    
    if (current_time < ready_time) {
        return std::nullopt;  // 아직 대기
    }
    
    auto input = front.command;
    last_processed_sequence_ = front.sequence;
    buffer_.pop_front();
    stats_.inputs_processed++;
    
    return input;
}

std::optional<InputCommand> InputBuffer::GetInput(std::uint32_t sequence) const {
    for (const auto& ti : buffer_) {
        if (ti.sequence == sequence) {
            return ti.command;
        }
    }
    return std::nullopt;
}

std::vector<InputCommand> InputBuffer::GetInputRange(
    std::uint32_t start_seq,
    std::uint32_t end_seq
) const {
    std::vector<InputCommand> result;
    for (const auto& ti : buffer_) {
        if (ti.sequence >= start_seq && ti.sequence <= end_seq) {
            result.push_back(ti.command);
        }
    }
    return result;
}

}  // namespace pvpserver::netcode
