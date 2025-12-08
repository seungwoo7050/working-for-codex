#include "pvpserver/distributed/consistent_hash.h"

#include <algorithm>
#include <sstream>

namespace pvpserver {
namespace distributed {

ConsistentHashRing::ConsistentHashRing(int virtual_nodes)
    : virtual_nodes_(virtual_nodes) {}

size_t ConsistentHashRing::Hash(const std::string& key) const {
    // FNV-1a hash
    size_t hash = 14695981039346656037ULL;
    for (char c : key) {
        hash ^= static_cast<size_t>(c);
        hash *= 1099511628211ULL;
    }
    return hash;
}

std::string ConsistentHashRing::MakeVirtualNodeKey(
    const std::string& node_id, int index) const {
    std::ostringstream oss;
    oss << node_id << "#" << index;
    return oss.str();
}

void ConsistentHashRing::AddNode(const std::string& node_id) {
    if (nodes_.count(node_id) > 0) {
        return;  // 이미 존재
    }

    nodes_.insert(node_id);

    // 가상 노드 추가
    for (int i = 0; i < virtual_nodes_; ++i) {
        std::string vnode_key = MakeVirtualNodeKey(node_id, i);
        size_t hash = Hash(vnode_key);
        ring_[hash] = node_id;
    }
}

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

std::string ConsistentHashRing::GetNode(const std::string& key) const {
    if (ring_.empty()) {
        return "";
    }

    size_t hash = Hash(key);
    auto it = ring_.lower_bound(hash);

    // 링 끝을 넘어가면 처음으로 돌아감
    if (it == ring_.end()) {
        it = ring_.begin();
    }

    return it->second;
}

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

    std::set<std::string> seen;
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

        // 전체 순회 완료
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
