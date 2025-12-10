// [FILE]
// - 목적: 매치 데이터 클래스 - 매칭된 플레이어 그룹 정보 저장
// - 주요 역할: 매치 ID, 참가자 목록, 평균 ELO, 생성 시간, 리전 관리
// - 관련 클론 가이드 단계: [CG-v1.2.0] 매치메이킹
// - 권장 읽는 순서: matchmaker.cpp에서 Match 생성 후 이 파일 참고
//
// [LEARN] 데이터 전송 객체(DTO) 패턴:
//         Match는 단순한 데이터 홀더로, 비즈니스 로직이 없다.
//         C의 struct와 유사하며, 생성자에서 모든 필드를 초기화한다.
//         std::move로 문자열/벡터의 불필요한 복사를 방지한다.

#include "pvpserver/matchmaking/match.h"

namespace pvpserver {

// [Order 1] Match 생성자 - 매치 정보 초기화
// - match_id: 고유 매치 식별자 ("match-123")
// - players: 참가자 ID 목록 (보통 2명)
// - average_elo: 참가자들의 평균 ELO 레이팅
// - created_at: 매치 생성 시간 (대기 시간 측정용)
// - region: 게임 서버 리전 ("kr", "us-west" 등)
Match::Match(std::string match_id, std::vector<std::string> players, int average_elo,
             std::chrono::steady_clock::time_point created_at, std::string region)
    : match_id_(std::move(match_id)),
      players_(std::move(players)),
      average_elo_(average_elo),
      created_at_(created_at),
      region_(std::move(region)) {}

}  // namespace pvpserver
