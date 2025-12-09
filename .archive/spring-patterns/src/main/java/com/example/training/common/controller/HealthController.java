package com.example.training.common.controller;

import com.example.training.common.dto.HealthResponse;
import org.springframework.http.ResponseEntity;
import org.springframework.web.bind.annotation.GetMapping;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RestController;

import java.time.LocalDateTime;

// [FILE]
// - 목적: 애플리케이션 상태 확인용 헬스체크 API 제공
// - 주요 역할: Kubernetes/Docker의 liveness probe 또는 로드밸런서 헬스체크 응답
// - 관련 클론 가이드 단계: [CG-v0.1.0] Bootstrap - 0.1.3 HealthController 구현
// - 권장 읽는 순서: health() 메서드 하나만 있음
//
// [LEARN] C 개발자를 위한 설명:
// Spring MVC의 Controller는 HTTP 요청을 처리하는 "핸들러 함수들의 모음"이다.
// C에서 HTTP 서버를 만들 때 URL 경로별로 핸들러 함수를 등록하고,
// 요청이 들어오면 파싱 후 해당 함수를 호출하는 구조와 동일하다.
// 다만 Spring에서는 어노테이션으로 URL 매핑을 선언적으로 정의한다.

@RestController
// [LEARN] @RestController = @Controller + @ResponseBody
// - @Controller: 이 클래스가 HTTP 요청 핸들러임을 선언
// - @ResponseBody: 메서드 반환값을 HTTP 응답 본문(JSON)으로 직접 변환
// C에서 응답 데이터를 JSON 문자열로 직렬화하는 코드를 직접 작성했다면,
// Spring은 Jackson 라이브러리로 객체 → JSON 변환을 자동 수행한다.

@RequestMapping("/api")
// [LEARN] @RequestMapping: 이 클래스의 모든 핸들러에 공통 URL 접두사 적용
// 아래 @GetMapping("/health")는 실제로 "/api/health"가 된다.
// C에서 라우터 테이블에 접두사를 붙여 등록하는 것과 동일한 개념.
public class HealthController {

    // [Order 1] 헬스체크 엔드포인트
    // - HTTP GET /api/health 요청을 처리
    // - 서버가 살아있으면 200 OK + 현재 시간 반환
    // - 클론 가이드 단계: [CG-v0.1.0] 0.1.3
    @GetMapping("/health")
    // [LEARN] @GetMapping: HTTP GET 메서드에만 반응하는 핸들러 등록
    // @PostMapping, @PutMapping, @DeleteMapping 등도 있다.
    // C에서 if (strcmp(method, "GET") == 0 && strcmp(path, "/health") == 0) 같은
    // 조건문을 어노테이션 하나로 대체한 것이다.
    public ResponseEntity<HealthResponse> health() {
        // [LEARN] ResponseEntity<T>: HTTP 응답의 상태코드, 헤더, 본문을 모두 제어
        // ResponseEntity.ok(body)는 상태코드 200과 함께 body를 JSON으로 반환한다.
        // C에서 send(socket, "HTTP/1.1 200 OK\r\n...", ...) 하던 것을 추상화한 것.
        HealthResponse response = new HealthResponse("OK", LocalDateTime.now());
        return ResponseEntity.ok(response);
    }
}

// [Reader Notes]
// ═══════════════════════════════════════════════════════════════════════════════
// 이 파일에서 처음 등장한 개념: REST Controller, 어노테이션 기반 라우팅
//
// Spring MVC 핵심 개념 (C 개발자 관점):
// ───────────────────────────────────────────────────────────────────────────────
// 1. Controller 패턴:
//    - HTTP 요청 → Controller 메서드 → 비즈니스 로직 호출 → 응답 반환
//    - C에서 main() → 라우터 → 핸들러 → 응답 전송 흐름과 동일
//    - 차이점: Spring은 URL 파싱, 파라미터 바인딩, JSON 변환을 자동 처리
//
// 2. DTO (Data Transfer Object):
//    - HealthResponse처럼 요청/응답 데이터를 담는 구조체
//    - C의 struct와 동일하지만, getter/setter 메서드가 추가됨
//    - Jackson이 이 구조를 읽어 자동으로 JSON으로 변환
//
// 3. 의존성 주입 (이 파일에서는 없지만):
//    - 복잡한 Controller는 Service 클래스를 생성자로 주입받음
//    - C에서 핸들러 함수에 컨텍스트 구조체 포인터를 전달하는 것과 유사
//
// 이 파일을 이해한 다음 볼 파일:
// ───────────────────────────────────────────────────────────────────────────────
// 1. common/exception/GlobalExceptionHandler.java - 전역 예외 처리
// 2. issue/controller/IssueController.java - 실제 CRUD API 구현
// ═══════════════════════════════════════════════════════════════════════════════
