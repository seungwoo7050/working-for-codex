package com.example.training.common.exception;

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * [FILE] ForbiddenException - 접근 권한 없음 예외
 * ─────────────────────────────────────────────────────────────────────────────────
 * - 목적: 인증된 사용자가 해당 리소스에 접근 권한이 없을 때 발생
 * - HTTP 상태코드: 403 Forbidden
 * - 관련 클론 가이드 단계: [CG-v1.1.0] 1.1.2 RBAC 예외 처리
 * 
 * [LEARN] 401 vs 403:
 * 
 * 401 Unauthorized (인증 실패):
 * - "당신이 누구인지 모르겠습니다"
 * - 로그인 필요, 토큰 만료/유효하지 않음
 * 
 * 403 Forbidden (인가 실패):
 * - "당신이 누구인지는 알지만, 권한이 없습니다"
 * - 로그인은 됐지만 해당 리소스 접근 권한 없음
 * 
 * RBAC 예시:
 *   MEMBER가 팀 삭제 시도 → 403 Forbidden
 *   (OWNER만 팀 삭제 가능)
 * 
 * C 관점:
 *   if (user->role < ROLE_MANAGER) {
 *       return ERROR_FORBIDDEN;  // 권한 부족
 *   }
 * ═══════════════════════════════════════════════════════════════════════════════
 */
public class ForbiddenException extends RuntimeException {
    
    public ForbiddenException(String message) {
        super(message);
    }
    // [LEARN] 사용 예:
    // if (member.getRole() != TeamRole.OWNER) {
    //     throw new ForbiddenException("팀 삭제는 OWNER만 가능합니다");
    // }
}
