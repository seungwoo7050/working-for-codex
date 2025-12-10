package com.example.training.team.repository;

import com.example.training.team.domain.TeamMember;
import com.example.training.team.domain.TeamRole;
import org.springframework.data.jpa.repository.JpaRepository;
import org.springframework.stereotype.Repository;

import java.util.List;
import java.util.Optional;

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * [FILE] TeamMemberRepository - 팀 멤버십 데이터 접근 계층
 * ─────────────────────────────────────────────────────────────────────────────────
 * - 목적: 팀-사용자 연관 테이블의 CRUD 및 조회 기능 제공
 * - 관련 클론 가이드 단계: [CG-v1.1.0] 1.1.1 RBAC 데이터 접근
 * 
 * [LEARN] 다양한 쿼리 메서드 예시:
 * 이 Repository는 다양한 Spring Data 쿼리 메서드 패턴을 보여줌
 * ═══════════════════════════════════════════════════════════════════════════════
 */
@Repository
public interface TeamMemberRepository extends JpaRepository<TeamMember, Long> {
    
    // ─────────────────────────────────────────────────────────────────────────
    // [Order 1] 단일 필드 검색
    // ─────────────────────────────────────────────────────────────────────────
    List<TeamMember> findByUserId(Long userId);
    // [LEARN] 사용자가 속한 모든 팀 멤버십 조회
    // SQL: SELECT * FROM team_members WHERE user_id = ?
    
    List<TeamMember> findByTeamId(Long teamId);
    // [LEARN] 특정 팀의 모든 멤버 조회
    // SQL: SELECT * FROM team_members WHERE team_id = ?

    // ─────────────────────────────────────────────────────────────────────────
    // [Order 2] 복합 조건 검색
    // ─────────────────────────────────────────────────────────────────────────
    Optional<TeamMember> findByTeamIdAndUserId(Long teamId, Long userId);
    // [LEARN] AND 조건: 특정 팀에서 특정 사용자의 멤버십 조회
    // SQL: SELECT * FROM team_members WHERE team_id = ? AND user_id = ?
    // Optional: 결과가 없을 수 있음 → null 대신 Optional.empty()
    
    // ─────────────────────────────────────────────────────────────────────────
    // [Order 3] 존재 여부 확인
    // ─────────────────────────────────────────────────────────────────────────
    boolean existsByTeamIdAndUserId(Long teamId, Long userId);
    // [LEARN] exists 쿼리: 데이터 가져오지 않고 존재 여부만 확인
    // SQL: SELECT EXISTS(SELECT 1 FROM team_members WHERE team_id = ? AND user_id = ?)
    // 전체 데이터를 가져오는 것보다 훨씬 효율적
    
    // ─────────────────────────────────────────────────────────────────────────
    // [Order 4] 집계 쿼리
    // ─────────────────────────────────────────────────────────────────────────
    int countByTeamIdAndRole(Long teamId, TeamRole role);
    // [LEARN] count 쿼리: 조건에 맞는 레코드 수 반환
    // SQL: SELECT COUNT(*) FROM team_members WHERE team_id = ? AND role = ?
    // 사용 예: OWNER 수 확인, MANAGER 수 제한 등
}
