package com.example.training.common.exception;

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * [FILE] ResourceNotFoundException - 리소스 미존재 예외
 * ─────────────────────────────────────────────────────────────────────────────────
 * - 목적: 요청한 리소스가 존재하지 않을 때 발생
 * - HTTP 상태코드: 404 Not Found
 * - 관련 클론 가이드 단계: [CG-v1.0.0] 1.0.4 예외 처리 설계
 * 
 * [LEARN] 비즈니스 예외 vs 시스템 예외:
 * 
 * 비즈니스 예외 (예상 가능):
 * - ResourceNotFoundException: 리소스 없음 → 404
 * - ForbiddenException: 권한 없음 → 403
 * - DuplicateResourceException: 중복 → 409
 * 
 * 시스템 예외 (예상 불가):
 * - NullPointerException, SQLException → 500
 * 
 * C 관점:
 *   int find_user(int id) {
 *       User* user = db_find(id);
 *       if (user == NULL) return ERROR_NOT_FOUND;  // 예상 가능
 *       // ...
 *   }
 * 
 * Java:
 *   throw new ResourceNotFoundException("User not found");
 *   → GlobalExceptionHandler가 404로 변환
 * ═══════════════════════════════════════════════════════════════════════════════
 */
public class ResourceNotFoundException extends RuntimeException {
    
    public ResourceNotFoundException(String message) {
        super(message);
    }
    // [LEARN] RuntimeException 상속:
    // - Unchecked Exception (명시적 catch 불필요)
    // - 컨트롤러까지 전파 → GlobalExceptionHandler에서 처리
}
