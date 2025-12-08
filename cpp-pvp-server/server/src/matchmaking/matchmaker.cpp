// [FILE]
// - 목적: ELO 기반 매치메이킹 시스템
// - 주요 역할: 플레이어 큐 관리, ELO 허용 범위 매칭, 매치 생성
// - 관련 클론 가이드 단계: [v1.2.0] 매치메이킹
// - 권장 읽는 순서: Enqueue → RunMatching → Match 생성
//
// [LEARN] 매치메이킹의 핵심은 "공정한 매칭"과 "대기 시간" 사이의 균형.
//         ELO 허용 범위를 시간에 따라 넓혀서 두 가지를 조화시킴.

#include "pvpserver/matchmaking/matchmaker.h"

#include <cmath>
#include <iostream>
#include <sstream>
#include <unordered_set>

namespace pvpserver {

// [Order 1] 생성자 - 매치메이킹 큐 연결
// - queue_: MatchQueue 인터페이스 (InMemory 또는 Redis 구현)
Matchmaker::Matchmaker(std::shared_ptr<MatchQueue> queue) : queue_(std::move(queue)) {}

void Matchmaker::SetMatchCreatedCallback(std::function<void(const Match&)> callback) {
    std::lock_guard<std::mutex> lk(mutex_);
    callback_ = std::move(callback);
}

// [Order 2] Enqueue - 플레이어를 매칭 대기열에 추가
// - 플레이어 ID, ELO 레이팅, 대기 시작 시간 저장
void Matchmaker::Enqueue(const MatchRequest& request) {
    std::size_t queue_size = 0;
    {
        std::lock_guard<std::mutex> lk(mutex_);
        queue_->Upsert(request, ++order_counter_);  // 순서 번호 부여
        last_queue_size_ = queue_->Size();
        queue_size = last_queue_size_;
    }
    std::cout << "matchmaking enqueue " << request.player_id() << " elo=" << request.elo()
              << " size=" << queue_size << std::endl;
}

// [Order 3] Cancel - 매칭 대기열에서 플레이어 제거
// - 연결 해제 또는 매칭 취소 시 호출
bool Matchmaker::Cancel(const std::string& player_id) {
    bool removed = false;
    std::size_t queue_size = 0;
    {
        std::lock_guard<std::mutex> lk(mutex_);
        removed = queue_->Remove(player_id);
        last_queue_size_ = queue_->Size();
        queue_size = last_queue_size_;
    }
    if (removed) {
        std::cout << "matchmaking cancel " << player_id << " size=" << queue_size << std::endl;
    }
    return removed;
}

// [Order 4] RunMatching - 핵심 매칭 알고리즘 실행
// - 대기열의 모든 플레이어를 순회하며 호환되는 상대 찾기
// - 200ms마다 main.cpp의 타이머에서 호출됨
// - 클론 가이드 단계: [v1.2.0]
std::vector<Match> Matchmaker::RunMatching(std::chrono::steady_clock::time_point now) {
    std::vector<Match> matches;
    std::function<void(const Match&)> callback;
    {
        std::lock_guard<std::mutex> lk(mutex_);
        auto ordered = queue_->FetchOrdered();  // 대기 순서대로 정렬된 목록
        std::unordered_set<std::string> used;   // 이번 라운드에서 매칭된 플레이어

        // O(N²) 알고리즘: 각 플레이어에 대해 호환되는 상대 탐색
        // [LEARN] 대규모 서비스에서는 ELO 버킷으로 분류하여 O(N) 근사 가능
        for (std::size_t i = 0; i < ordered.size(); ++i) {
            const auto& candidate = ordered[i];
            const auto& request = candidate.request;
            if (used.find(request.player_id()) != used.end()) {
                continue;  // 이미 매칭됨
            }

            // 현재 플레이어의 ELO 허용 범위 계산
            // [LEARN] CurrentTolerance: 대기 시간에 따라 허용 범위 확대
            //         초기 ±100 → 5초마다 ±25씩 증가
            const int tolerance_a = request.CurrentTolerance(now);
            std::size_t partner_index = ordered.size();

            // 호환되는 상대 탐색
            for (std::size_t j = i + 1; j < ordered.size(); ++j) {
                const auto& other = ordered[j];
                if (used.find(other.request.player_id()) != used.end()) {
                    continue;  // 이미 매칭됨
                }
                // 리전 호환성 체크 (같은 서버 지역)
                if (!RegionsCompatible(request, other.request)) {
                    continue;
                }
                // ELO 차이 계산 + 양쪽 허용 범위 모두 만족해야 함
                const int diff = std::abs(request.elo() - other.request.elo());
                const int tolerance_b = other.request.CurrentTolerance(now);
                if (diff <= tolerance_a && diff <= tolerance_b) {
                    partner_index = j;
                    break;  // 첫 번째 호환 상대 선택 (가장 오래 대기한 순)
                }
                // 조기 종료: ELO 정렬되어 있으면 더 이상 볼 필요 없음
                if (other.request.elo() - request.elo() > tolerance_a) {
                    break;
                }
            }
            if (partner_index >= ordered.size()) {
                continue;  // 호환 상대 없음
            }
            const auto& partner = ordered[partner_index].request;

            // 매칭 성사: 양쪽 큐에서 제거 + 사용 표시
            queue_->Remove(request.player_id());
            queue_->Remove(partner.player_id());
            used.insert(request.player_id());
            used.insert(partner.player_id());

            // Match 객체 생성
            ++matches_created_;
            const int average_elo = (request.elo() + partner.elo()) / 2;
            std::ostringstream id_stream;
            id_stream << "match-" << ++match_counter_;
            Match match(id_stream.str(), {request.player_id(), partner.player_id()}, average_elo,
                        now, ResolveRegion(request, partner));
            matches.push_back(match);

            // 대기 시간 히스토그램 기록 (Prometheus 메트릭용)
            ObserveWaitLocked(request.WaitSeconds(now));
            ObserveWaitLocked(partner.WaitSeconds(now));
        }
        last_queue_size_ = queue_->Size();
        callback = callback_;
    }

    // 생성된 매치 알림 (잠금 해제 후)
    for (const auto& match : matches) {
        std::cout << "matchmaking match " << match.match_id() << " players=" << match.players()[0]
                  << ',' << match.players()[1] << " elo=" << match.average_elo() << std::endl;
        notifications_.Publish(match);  // 옵저버 패턴으로 알림
        if (callback) {
            callback(match);
        }
    }

    return matches;
}

std::string Matchmaker::MetricsSnapshot() const {
    std::lock_guard<std::mutex> lk(mutex_);
    std::ostringstream oss;
    oss << "# TYPE matchmaking_queue_size gauge\n";
    oss << "matchmaking_queue_size " << last_queue_size_ << "\n";
    oss << "# TYPE matchmaking_matches_total counter\n";
    oss << "matchmaking_matches_total " << matches_created_ << "\n";
    oss << "# TYPE matchmaking_wait_seconds histogram\n";
    std::uint64_t cumulative = 0;
    for (std::size_t i = 0; i < kWaitBuckets.size(); ++i) {
        cumulative += wait_bucket_counts_[i];
        oss << "matchmaking_wait_seconds_bucket{le=\"" << kWaitBuckets[i] << "\"} " << cumulative
            << "\n";
    }
    cumulative += wait_overflow_count_;
    oss << "matchmaking_wait_seconds_bucket{le=\"+Inf\"} " << cumulative << "\n";
    oss << "matchmaking_wait_seconds_sum " << wait_sum_ << "\n";
    oss << "matchmaking_wait_seconds_count " << wait_count_ << "\n";
    return oss.str();
}

void Matchmaker::ObserveWaitLocked(double seconds) {
    wait_sum_ += seconds;
    ++wait_count_;
    bool bucket_found = false;
    for (std::size_t i = 0; i < kWaitBuckets.size(); ++i) {
        if (seconds <= kWaitBuckets[i]) {
            ++wait_bucket_counts_[i];
            bucket_found = true;
            break;
        }
    }
    if (!bucket_found) {
        ++wait_overflow_count_;
    }
}

std::string Matchmaker::ResolveRegion(const MatchRequest& lhs, const MatchRequest& rhs) {
    if (lhs.preferred_region() == rhs.preferred_region()) {
        return lhs.preferred_region();
    }
    if (lhs.preferred_region() == "any") {
        return rhs.preferred_region();
    }
    if (rhs.preferred_region() == "any") {
        return lhs.preferred_region();
    }
    return lhs.preferred_region();
}

}  // namespace pvpserver

// [Reader Notes]
// ================================================================================
// 이 파일에서 처음 등장한 게임 서버 개념:
//
// 1. ELO 기반 매치메이킹
//    - ELO: 체스에서 유래한 상대적 실력 지표 (기본값 1200)
//    - 허용 범위(tolerance)로 비슷한 실력끼리 매칭
//    - 대기 시간 증가 시 허용 범위 확대 (공정성 vs 대기시간 트레이드오프)
//
// 2. 동적 허용 범위
//    - 초기: ±100 ELO 차이까지 허용
//    - 5초마다: ±25씩 허용 범위 확대
//    - 오래 기다릴수록 넓은 실력 범위에서 매칭
//
// 3. Prometheus 히스토그램
//    - 대기 시간 분포를 버킷으로 기록
//    - kWaitBuckets: [1, 5, 10, 30, 60, 120, 300] 초
//    - 서비스 품질(QoS) 모니터링에 활용
//
// 관련 설계 문서:
// - design/v1.2.0-matchmaking.md (매치메이킹 알고리즘)
//
// 이 파일을 이해한 다음, 이어서 보면 좋은 파일:
// - server/src/matchmaking/match_queue.cpp (대기열 구현)
// - server/src/matchmaking/match_request.cpp (요청 데이터)
// - server/src/stats/player_profile_service.cpp (ELO 업데이트)
// ================================================================================
