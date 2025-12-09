// [FILE]
// - 목적: Consistent Hashing(일관적 해싱)을 이용한 분산 시스템 라우팅
// - 주요 역할: 세션/플레이어를 서버 노드에 균등 분배, 노드 추가/제거 시 최소 재배치
// - 관련 클론 가이드 단계: [CG-02.00] 분산 시스템
// - 권장 읽는 순서: Hash → AddNode → GetNode → GetNodes
//
// [LEARN] Consistent Hashing은 분산 시스템의 핵심 알고리즘.
//         일반 해싱은 노드 추가/제거 시 거의 모든 키가 재배치되지만,
//         Consistent Hashing은 평균 K/N개만 이동 (K=키 수, N=노드 수).
//         가상 노드(Virtual Node)로 부하 균등화 향상.

#include "pvpserver/distributed/consistent_hash.h"

#include <algorithm>
#include <sstream>

namespace pvpserver {
namespace distributed {

// [Order 1] 생성자 - 가상 노드 수 설정
// - virtual_nodes_: 실제 노드당 가상 노드 수 (기본 150)
// - 가상 노드가 많을수록 부하 분산이 균등하지만 메모리 사용 증가
ConsistentHashRing::ConsistentHashRing(int virtual_nodes)
    : virtual_nodes_(virtual_nodes) {}

// [Order 2] Hash - FNV-1a 해시 함수
// [LEARN] FNV-1a는 빠르고 분포가 좋은 비암호화 해시.
//         게임 서버에서는 MurmurHash3도 많이 사용.
//         암호화 해시(SHA)보다 훨씬 빠름.
size_t ConsistentHashRing::Hash(const std::string& key) const {
    // FNV-1a 해시 초기값과 프라임
    size_t hash = 14695981039346656037ULL;
    for (char c : key) {
        hash ^= static_cast<size_t>(c);  // XOR
        hash *= 1099511628211ULL;        // FNV 프라임
    }
    return hash;
}

// 가상 노드 키 생성: "node-1#0", "node-1#1", ...
std::string ConsistentHashRing::MakeVirtualNodeKey(
    const std::string& node_id, int index) const {
    std::ostringstream oss;
    oss << node_id << "#" << index;
    return oss.str();
}

// [Order 3] AddNode - 노드 추가
// - 실제 노드 1개 추가 시 가상 노드 virtual_nodes_개가 링에 배치됨
// [LEARN] 가상 노드가 링에 분산되어 있어서 부하가 균등해짐.
//         가상 노드 없이 실제 노드만 사용하면 노드 간 부하 편차가 큼.
void ConsistentHashRing::AddNode(const std::string& node_id) {
    if (nodes_.count(node_id) > 0) {
        return;  // 이미 존재
    }

    nodes_.insert(node_id);

    // 가상 노드들을 링에 추가
    for (int i = 0; i < virtual_nodes_; ++i) {
        std::string vnode_key = MakeVirtualNodeKey(node_id, i);
        size_t hash = Hash(vnode_key);
        ring_[hash] = node_id;  // 해시값 → 실제 노드 ID 매핑
    }
}

// [Order 4] RemoveNode - 노드 제거
// - 해당 노드의 모든 가상 노드를 링에서 제거
// - 제거된 가상 노드가 담당하던 키들은 다음 노드로 이동
void ConsistentHashRing::RemoveNode(const std::string& node_id) {
    if (nodes_.count(node_id) == 0) {
        return;  // 존재하지 않음
    }

    nodes_.erase(node_id);

    // 가상 노드 제거
    for (int i = 0; i < virtual_nodes_; ++i) {
        std::string vnode_key = MakeVirtualNodeKey(node_id, i);
        size_t hash = Hash(vnode_key);
        ring_.erase(hash);
    }
}

// [Order 5] GetNode - 키에 해당하는 노드 조회
// [LEARN] 키의 해시값보다 크거나 같은 첫 번째 가상 노드 선택.
//         링 끝을 넘어가면 처음으로 돌아감 (원형 구조).
std::string ConsistentHashRing::GetNode(const std::string& key) const {
    if (ring_.empty()) {
        return "";
    }

    size_t hash = Hash(key);
    auto it = ring_.lower_bound(hash);  // hash 이상인 첫 번째 항목

    // 링 끝을 넘어가면 처음으로 돌아감 (원형 링)
    if (it == ring_.end()) {
        it = ring_.begin();
    }

    return it->second;  // 실제 노드 ID 반환
}

// [Order 6] GetNodes - 복제용 N개 노드 조회
// - 키에 대해 담당 노드 + 복제 노드 N개 반환
// - 고가용성(HA) 구현에 사용
std::vector<std::string> ConsistentHashRing::GetNodes(
    const std::string& key, int n) const {
    std::vector<std::string> result;

    if (ring_.empty() || n <= 0) {
        return result;
    }

    // 실제 노드 수보다 많이 요청하면 전체 반환
    int max_nodes = std::min(n, static_cast<int>(nodes_.size()));

    size_t hash = Hash(key);
    auto it = ring_.lower_bound(hash);
    if (it == ring_.end()) {
        it = ring_.begin();
    }

    std::set<std::string> seen;  // 중복 방지 (가상 노드가 같은 실제 노드일 수 있음)
    auto start_it = it;
    bool wrapped = false;

    while (static_cast<int>(result.size()) < max_nodes) {
        if (seen.count(it->second) == 0) {
            result.push_back(it->second);
            seen.insert(it->second);
        }

        ++it;
        if (it == ring_.end()) {
            it = ring_.begin();
            wrapped = true;
        }

        // 전체 순회 완료 (무한 루프 방지)
        if (wrapped && it == start_it) {
            break;
        }
    }

    return result;
}

size_t ConsistentHashRing::GetNodeCount() const {
    return nodes_.size();
}

bool ConsistentHashRing::HasNode(const std::string& node_id) const {
    return nodes_.count(node_id) > 0;
}

std::vector<std::string> ConsistentHashRing::GetAllNodes() const {
    return std::vector<std::string>(nodes_.begin(), nodes_.end());
}

std::map<std::string, int> ConsistentHashRing::GetDistribution(
    const std::vector<std::string>& keys) const {
    std::map<std::string, int> distribution;

    // 모든 노드 초기화
    for (const auto& node : nodes_) {
        distribution[node] = 0;
    }

    // 키별 분배 계산
    for (const auto& key : keys) {
        std::string node = GetNode(key);
        if (!node.empty()) {
            ++distribution[node];
        }
    }

    return distribution;
}

}  // namespace distributed
}  // namespace pvpserver

// [Reader Notes]
// ================================================================================
// 이 파일에서 처음 등장한 분산 시스템 개념:
//
// 1. Consistent Hashing (일관적 해싱)
//    - 해시 값을 원형 링에 배치하는 분산 라우팅 기법
//    - 노드 추가/제거 시 평균 K/N개 키만 재배치 (K=키 수, N=노드 수)
//    - 일반 해싱(hash % N)은 N 변경 시 거의 모든 키 재배치
//
// 2. 가상 노드 (Virtual Node)
//    - 실제 노드 1개를 링에 여러 개 배치 (예: 150개)
//    - 부하 균등화 향상: 노드 간 편차 감소
//    - 트레이드오프: 메모리 사용량 증가
//
// 3. FNV-1a 해시
//    - Fast, Non-cryptographic, Varied: 빠르고 분포 좋은 해시
//    - 게임 서버에서는 MurmurHash3도 많이 사용
//    - SHA/MD5 같은 암호화 해시보다 10-100배 빠름
//
// 관련 설계 문서:
// - design/v2.0.0-session-store.md (분산 세션)
// - design/v2.0.1-load-balancer.md (로드 밸런싱)
//
// 이 파일을 이해한 다음, 이어서 보면 좋은 파일:
// - server/src/distributed/load_balancer.cpp (로드 밸런서)
// - server/src/distributed/service_discovery.cpp (서비스 발견)
// ================================================================================
