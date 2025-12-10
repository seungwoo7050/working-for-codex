#pragma once

#include <functional>
#include <map>
#include <set>
#include <string>
#include <vector>

namespace pvpserver {
namespace distributed {

/**
 * Consistent Hash Ring
 * 가상 노드를 사용하여 균등한 분배를 보장합니다.
 */
class ConsistentHashRing {
public:
    /**
     * 생성자
     * @param virtual_nodes 물리 노드당 가상 노드 수 (기본 150)
     */
    explicit ConsistentHashRing(int virtual_nodes = 150);
    ~ConsistentHashRing() = default;

    /**
     * 노드 추가
     * @param node_id 노드 식별자
     */
    void AddNode(const std::string& node_id);

    /**
     * 노드 제거
     * @param node_id 노드 식별자
     */
    void RemoveNode(const std::string& node_id);

    /**
     * 키에 대한 노드 조회
     * @param key 조회 키 (예: player_id)
     * @return 담당 노드 ID (노드 없으면 빈 문자열)
     */
    std::string GetNode(const std::string& key) const;

    /**
     * 키에 대한 N개 노드 조회 (복제 또는 페일오버용)
     * @param key 조회 키
     * @param n 조회할 노드 수
     * @return 노드 ID 목록
     */
    std::vector<std::string> GetNodes(const std::string& key, int n) const;

    /**
     * 현재 노드 수
     * @return 물리 노드 수
     */
    size_t GetNodeCount() const;

    /**
     * 노드 존재 여부
     * @param node_id 노드 식별자
     * @return 존재 여부
     */
    bool HasNode(const std::string& node_id) const;

    /**
     * 모든 노드 조회
     * @return 노드 ID 목록
     */
    std::vector<std::string> GetAllNodes() const;

    /**
     * 분배 분석 (테스트/디버깅용)
     * @param keys 테스트 키 목록
     * @return 노드별 키 할당 수
     */
    std::map<std::string, int> GetDistribution(
        const std::vector<std::string>& keys) const;

private:
    int virtual_nodes_;
    std::map<size_t, std::string> ring_;      // hash -> node_id
    std::set<std::string> nodes_;              // 물리 노드 집합

    /**
     * 문자열 해시 (FNV-1a)
     */
    size_t Hash(const std::string& key) const;

    /**
     * 가상 노드 키 생성
     */
    std::string MakeVirtualNodeKey(const std::string& node_id, int index) const;
};

}  // namespace distributed
}  // namespace pvpserver
