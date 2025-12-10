package com.example.training.team.domain;

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * [FILE] TeamRole Enum - RBAC 역할 정의
 * ─────────────────────────────────────────────────────────────────────────────────
 * - 목적: 팀 내 사용자 역할(권한 수준) 정의
 * - 관련 클론 가이드 단계: [CG-v1.1.0] 1.1.1 RBAC 역할 설계
 * 
 * [LEARN] RBAC (Role-Based Access Control):
 * 역할 기반 접근 제어 - 사용자에게 역할을 부여하고, 역할에 권한을 매핑
 * 
 * C 관점:
 *   #define ROLE_OWNER   0x04  // 100 (모든 권한)
 *   #define ROLE_MANAGER 0x02  // 010 (중간 권한)
 *   #define ROLE_MEMBER  0x01  // 001 (기본 권한)
 * 
 *   int can_delete(int role) {
 *       return role >= ROLE_MANAGER;  // MANAGER 이상만 삭제 가능
 *   }
 * 
 * Java Enum: 타입 안전성 + 가독성
 *   if (role == TeamRole.OWNER) { ... }
 * ═══════════════════════════════════════════════════════════════════════════════
 */
public enum TeamRole {
    
    // ─────────────────────────────────────────────────────────────────────────
    // [LEARN] Enum 값 정의: 권한 수준 내림차순
    // ─────────────────────────────────────────────────────────────────────────
    
    OWNER,
    // [LEARN] 팀 소유자: 최고 권한
    // - 팀 삭제, 멤버 추가/삭제, 역할 변경 모두 가능
    // - 팀당 1명만 존재 (보통)
    
    MANAGER,
    // [LEARN] 팀 관리자: 중간 권한
    // - 멤버 추가/삭제 가능
    // - 팀 삭제, OWNER 권한 변경 불가
    
    MEMBER
    // [LEARN] 팀 멤버: 기본 권한
    // - 팀 리소스 조회/생성만 가능
    // - 멤버 관리 권한 없음
    
    // ─────────────────────────────────────────────────────────────────────────
    // [LEARN] Enum 비교:
    // 
    // ordinal() 사용 (권장하지 않음):
    //   role.ordinal() >= TeamRole.MANAGER.ordinal()
    //   → Enum 순서 바뀌면 버그 발생
    // 
    // 명시적 비교 (권장):
    //   role == TeamRole.OWNER || role == TeamRole.MANAGER
    //   또는 EnumSet 사용
    //
    // TeamService에서 사용 예:
    //   if (member.getRole() != TeamRole.OWNER) {
    //       throw new ForbiddenException("팀 삭제는 OWNER만 가능");
    //   }
    // ─────────────────────────────────────────────────────────────────────────
}
