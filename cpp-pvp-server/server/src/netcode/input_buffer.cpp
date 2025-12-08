// [FILE]
// - 목적: 입력 버퍼 (클라이언트 입력 지연 처리)
// - 주요 역할: 네트워크 지터 대응, 입력 순서 보장
// - 관련 클론 가이드 단계: [v1.4.0] UDP 넷코드
// - 권장 읽는 순서: Push → Pop → GetInputRange
//
// [LEARN] 입력 버퍼링 이유:
//         - 네트워크 지터(jitter): 패킷 도착 시간 변동
//         - 패킷 순서 뒤바뀜: UDP는 순서 보장 안 함
//         - 버퍼 딜레이: 약간의 지연으로 안정적 재생

#include "pvpserver/netcode/input_buffer.h"

#include <algorithm>

namespace pvpserver::netcode {

// [Order 1] Push - 입력 추가 (정렬된 삽입)
// [LEARN] 시퀀스 번호 기준 정렬 삽입.
//         UDP 패킷이 순서대로 오지 않아도 올바른 순서 유지.
void InputBuffer::Push(const TimestampedInput& input) {
    stats_.inputs_received++;
    
    // 오래된 입력 무시 (이미 처리된 시퀀스)
    if (input.sequence <= last_processed_sequence_) {
        stats_.inputs_dropped++;
        return;
    }
    
    // 이진 탐색으로 삽입 위치 찾기 (정렬 유지)
    auto it = std::lower_bound(
        buffer_.begin(), buffer_.end(), input,
        [](const TimestampedInput& a, const TimestampedInput& b) {
            return a.sequence < b.sequence;
        }
    );
    
    // 중복 체크
    if (it != buffer_.end() && it->sequence == input.sequence) {
        return;  // 이미 존재하는 시퀀스
    }
    
    buffer_.insert(it, input);  // 정렬된 위치에 삽입
    
    // 버퍼 크기 제한 (메모리 보호)
    while (buffer_.size() > BUFFER_SIZE) {
        buffer_.pop_front();
        stats_.inputs_dropped++;
    }
}

// [Order 2] Pop - 다음 입력 꺼내기
// [LEARN] 버퍼 딜레이(buffer_delay_ms_)만큼 대기 후 반환.
//         지터 흡수용 - 약간의 지연으로 안정적 재생.
std::optional<InputCommand> InputBuffer::Pop(std::uint64_t current_time) {
    if (buffer_.empty()) {
        return std::nullopt;
    }
    
    // 버퍼 딜레이 적용 (도착 시간 + 딜레이 < 현재 시간이면 준비됨)
    const auto& front = buffer_.front();
    std::uint64_t ready_time = front.server_receive_time + buffer_delay_ms_;
    
    if (current_time < ready_time) {
        return std::nullopt;  // 아직 대기 중
    }
    
    auto input = front.command;
    last_processed_sequence_ = front.sequence;
    buffer_.pop_front();
    stats_.inputs_processed++;
    
    return input;
}

// GetInput - 특정 시퀀스 입력 조회 (리컨실리에이션용)
std::optional<InputCommand> InputBuffer::GetInput(std::uint32_t sequence) const {
    for (const auto& ti : buffer_) {
        if (ti.sequence == sequence) {
            return ti.command;
        }
    }
    return std::nullopt;
}

// [Order 3] GetInputRange - 범위 내 입력 조회 (재시뮬레이션용)
// [LEARN] 리컨실리에이션 시 미확인 입력 범위 가져오기
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
