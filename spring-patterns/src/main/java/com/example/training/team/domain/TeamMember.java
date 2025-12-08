package com.example.training.team.domain;

import jakarta.persistence.*;
import java.time.LocalDateTime;

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * [FILE] TeamMember 엔티티 - 팀-사용자 다대다 관계의 중간 테이블
 * ─────────────────────────────────────────────────────────────────────────────────
 * - 목적: 사용자의 팀 소속 정보와 역할(Role)을 저장
 * - 관련 클론 가이드 단계: [CG-v1.1.0] 1.1.1 RBAC 도메인 설계
 * 
 * [LEARN] 연관 테이블(Association Table) 패턴:
 * Team ↔ User 다대다(M:N) 관계를 일대다(1:N) 두 개로 분해
 * - 한 팀에 여러 사용자 (1:N)
 * - 한 사용자가 여러 팀 소속 (1:N)
 * 중간 테이블에 추가 정보(role, joinedAt)를 저장할 수 있음
 * ═══════════════════════════════════════════════════════════════════════════════
 */
@Entity
@Table(name = "team_members", uniqueConstraints = {
    @UniqueConstraint(columnNames = {"team_id", "user_id"})
})
// ─────────────────────────────────────────────────────────────────────────────
// [LEARN] @UniqueConstraint: 복합 유니크 제약조건
// team_id + user_id 조합이 중복 불가 → 같은 팀에 같은 사용자 두 번 가입 방지
// DDL: UNIQUE KEY uk_team_user (team_id, user_id)
// ─────────────────────────────────────────────────────────────────────────────
public class TeamMember {

    @Id
    @GeneratedValue(strategy = GenerationType.IDENTITY)
    private Long id;

    @Column(name = "team_id", nullable = false)
    private Long teamId;
    // [LEARN] @JoinColumn 대신 단순 Long으로 FK 저장
    // 연관 엔티티가 필요할 때는 별도 쿼리로 조회 (성능상 장점)

    @Column(name = "user_id", nullable = false)
    private Long userId;

    // ─────────────────────────────────────────────────────────────────────────
    // [Order 3] Enum 매핑 - 역할(Role) 저장
    // ─────────────────────────────────────────────────────────────────────────
    @Enumerated(EnumType.STRING)
    @Column(nullable = false)
    private TeamRole role;
    // [LEARN] @Enumerated(EnumType.STRING): Enum을 문자열로 저장
    // - ORDINAL: 0, 1, 2 숫자로 저장 (Enum 순서 바뀌면 위험!)
    // - STRING: "OWNER", "MANAGER", "MEMBER" 문자열로 저장 (안전)
    //
    // C 관점: enum { OWNER=0, MANAGER=1, MEMBER=2 } 를
    //         VARCHAR 컬럼에 "OWNER" 문자열로 저장

    @Column(nullable = false, updatable = false)
    private LocalDateTime joinedAt;
    // [LEARN] 팀 가입 일시: 언제 이 팀에 합류했는지 기록
    // RBAC에서 "선임 멤버" 등의 로직에 활용 가능

    @PrePersist
    protected void onCreate() {
        joinedAt = LocalDateTime.now();
    }

    // ─────────────────────────────────────────────────────────────────────────
    // [Order 4] 생성자
    // ─────────────────────────────────────────────────────────────────────────
    protected TeamMember() {
    }

    public TeamMember(Long teamId, Long userId, TeamRole role) {
        this.teamId = teamId;
        this.userId = userId;
        this.role = role;
    }
    // [LEARN] 팀 멤버 생성: 어떤 팀(teamId)에 어떤 사용자(userId)가
    // 어떤 역할(role)로 가입하는지 명시

    // ─────────────────────────────────────────────────────────────────────────
    // [Order 5] Getter/Setter
    // ─────────────────────────────────────────────────────────────────────────
    public Long getId() {
        return id;
    }

    public Long getTeamId() {
        return teamId;
    }

    public Long getUserId() {
        return userId;
    }

    public TeamRole getRole() {
        return role;
    }

    public LocalDateTime getJoinedAt() {
        return joinedAt;
    }

    public void setRole(TeamRole role) {
        this.role = role;
    }
    // [LEARN] role만 setter 제공: 역할 변경(승격/강등)은 허용
    // teamId, userId는 변경 불가 → 이동하려면 삭제 후 재생성
}
