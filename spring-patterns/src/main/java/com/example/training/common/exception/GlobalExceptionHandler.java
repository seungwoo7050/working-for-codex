package com.example.training.common.exception;

import com.example.training.common.dto.ErrorResponse;
import org.springframework.http.HttpStatus;
import org.springframework.http.ResponseEntity;
import org.springframework.validation.FieldError;
import org.springframework.web.bind.MethodArgumentNotValidException;
import org.springframework.web.bind.annotation.ExceptionHandler;
import org.springframework.web.bind.annotation.RestControllerAdvice;

import java.util.stream.Collectors;

// [FILE]
// - 목적: 애플리케이션 전역에서 발생하는 예외를 일관된 형식으로 처리
// - 주요 역할: 예외 유형별로 적절한 HTTP 상태 코드와 에러 메시지 반환
// - 관련 클론 가이드 단계: [CG-v0.1.0] 0.1.4 GlobalExceptionHandler
// - 권장 읽는 순서: Order 1~5 순서대로 (예외 유형별 핸들러)
//
// [LEARN] C 개발자를 위한 설명:
// C에서 에러 처리는 보통 함수 반환값으로 에러 코드를 전달하고,
// 호출자가 이를 확인하여 적절히 처리하는 방식이다.
// Java/Spring에서는 예외(Exception)를 throw하고, 상위에서 catch한다.
//
// 이 클래스는 "AOP(Aspect-Oriented Programming)" 방식으로 동작한다:
// 모든 Controller에서 예외가 발생하면, Spring이 이 클래스의 핸들러를 자동 호출한다.
// C에서 setjmp/longjmp나 signal handler로 전역 에러 처리를 구현하는 것과 유사하다.

@RestControllerAdvice
// [LEARN] @RestControllerAdvice: 모든 @RestController에 공통 적용되는 "어드바이스"
// - @ControllerAdvice: Controller 횡단 관심사(cross-cutting concern) 처리
// - @ResponseBody: 반환값을 JSON으로 변환
// 모든 Controller의 예외가 이 클래스로 "위임"되어 처리된다.
public class GlobalExceptionHandler {

    // [Order 1] 리소스를 찾을 수 없을 때 (404 Not Found)
    // - 존재하지 않는 ID로 조회 시 발생
    // - 클론 가이드 단계: [CG-v1.0.0] CRUD 구현 시 필수
    @ExceptionHandler(ResourceNotFoundException.class)
    // [LEARN] @ExceptionHandler: 특정 예외 타입을 처리하는 메서드 지정
    // ResourceNotFoundException이 throw되면 이 메서드가 호출된다.
    // C에서 switch(error_code) { case NOT_FOUND: ... } 패턴과 유사.
    public ResponseEntity<ErrorResponse> handleResourceNotFound(ResourceNotFoundException ex) {
        ErrorResponse error = new ErrorResponse("RESOURCE_NOT_FOUND", ex.getMessage());
        return ResponseEntity.status(HttpStatus.NOT_FOUND).body(error);
    }

    // [Order 2] 중복 리소스 (409 Conflict)
    // - 이미 존재하는 이메일로 회원가입 시도 등
    @ExceptionHandler(DuplicateResourceException.class)
    public ResponseEntity<ErrorResponse> handleDuplicateResource(DuplicateResourceException ex) {
        ErrorResponse error = new ErrorResponse("DUPLICATE_RESOURCE", ex.getMessage());
        return ResponseEntity.status(HttpStatus.CONFLICT).body(error);
    }

    // [Order 3] 인증 실패 (401 Unauthorized)
    // - 로그인 실패, 잘못된 비밀번호 등
    // - 클론 가이드 단계: [CG-v1.1.0] Security 구현 시 사용
    @ExceptionHandler(UnauthorizedException.class)
    public ResponseEntity<ErrorResponse> handleUnauthorized(UnauthorizedException ex) {
        ErrorResponse error = new ErrorResponse("UNAUTHORIZED", ex.getMessage());
        return ResponseEntity.status(HttpStatus.UNAUTHORIZED).body(error);
    }

    // [Order 4] 권한 없음 (403 Forbidden)
    // - 로그인은 했지만 해당 리소스에 접근 권한이 없을 때
    // - 클론 가이드 단계: [CG-v1.1.0] RBAC 구현 시 사용
    @ExceptionHandler(ForbiddenException.class)
    public ResponseEntity<ErrorResponse> handleForbidden(ForbiddenException ex) {
        ErrorResponse error = new ErrorResponse("FORBIDDEN", ex.getMessage());
        return ResponseEntity.status(HttpStatus.FORBIDDEN).body(error);
    }

    // [Order 5] 입력 유효성 검증 실패 (400 Bad Request)
    // - @Valid 어노테이션으로 DTO 검증 시 실패하면 발생
    // - 클론 가이드 단계: [CG-v1.0.0] DTO 유효성 검증
    @ExceptionHandler(MethodArgumentNotValidException.class)
    public ResponseEntity<ErrorResponse> handleValidationException(MethodArgumentNotValidException ex) {
        // [LEARN] Stream API: Java 8+의 함수형 프로그래밍 스타일
        // C에서 for 루프로 배열을 순회하며 처리하는 것을
        // .stream().map().collect() 체인으로 표현한다.
        String message = ex.getBindingResult().getFieldErrors().stream()
                .map(FieldError::getDefaultMessage)
                .collect(Collectors.joining(", "));
        ErrorResponse error = new ErrorResponse("VALIDATION_ERROR", message);
        return ResponseEntity.status(HttpStatus.BAD_REQUEST).body(error);
    }

    // [Order 6] 기타 모든 예외 (500 Internal Server Error)
    // - 예상치 못한 서버 에러에 대한 폴백 핸들러
    // - 프로덕션에서는 상세 메시지를 숨기고 로깅만 수행해야 함
    @ExceptionHandler(Exception.class)
    public ResponseEntity<ErrorResponse> handleGenericException(Exception ex) {
        // [LEARN] 실무에서는 여기서 ex를 로깅하고,
        // 클라이언트에는 민감한 정보가 노출되지 않도록 일반 메시지만 반환한다.
        ErrorResponse error = new ErrorResponse("INTERNAL_ERROR", "An unexpected error occurred");
        return ResponseEntity.status(HttpStatus.INTERNAL_SERVER_ERROR).body(error);
    }
}
