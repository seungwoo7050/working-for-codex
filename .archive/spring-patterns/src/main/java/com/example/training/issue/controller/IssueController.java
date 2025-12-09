package com.example.training.issue.controller;

import com.example.training.issue.domain.IssueStatus;
import com.example.training.issue.dto.IssueRequest;
import com.example.training.issue.dto.IssueResponse;
import com.example.training.issue.dto.IssueUpdateRequest;
import com.example.training.issue.service.IssueService;
import jakarta.validation.Valid;
import org.springframework.data.domain.Page;
import org.springframework.data.domain.PageRequest;
import org.springframework.data.domain.Pageable;
import org.springframework.http.HttpStatus;
import org.springframework.http.ResponseEntity;
import org.springframework.security.core.Authentication;
import org.springframework.web.bind.annotation.*;

import java.util.List;

// [FILE]
// - 목적: Issue 관련 REST API 엔드포인트 정의
// - 주요 역할: HTTP 요청을 받아 Service 호출 후 응답 반환
// - 관련 클론 가이드 단계: [CG-v1.0.0] 1.0.4 Controller Layer
// - 권장 읽는 순서: Order 1(생성) → Order 2(목록조회) → Order 3(단건조회) → Order 4(수정) → Order 5(삭제)
//
// [LEARN] C 개발자를 위한 설명:
// Controller는 "HTTP 요청 라우터 + 핸들러 함수 모음"이다.
// C에서 HTTP 서버를 구현할 때:
// 1. URL 파싱
// 2. 요청 본문 파싱 (JSON → struct)
// 3. 핸들러 함수 호출
// 4. 응답 생성 (struct → JSON)
// 5. HTTP 응답 전송
//
// Spring MVC는 이 모든 과정을 어노테이션과 프레임워크가 처리하고,
// 개발자는 "어떤 URL에 어떤 로직을 실행할지"만 선언한다.

@RestController
@RequestMapping("/api")
// [LEARN] 클래스 레벨 @RequestMapping: 모든 메서드의 URL 접두사
public class IssueController {

    private final IssueService issueService;

    public IssueController(IssueService issueService) {
        // [LEARN] 생성자 주입: IssueService Bean을 자동으로 주입받음
        // Controller는 Service만 알고, Repository는 모름 (레이어 분리)
        this.issueService = issueService;
    }

    // ═══════════════════════════════════════════════════════════════════════
    // [Order 1] 이슈 생성 - POST /api/projects/{projectId}/issues
    // ═══════════════════════════════════════════════════════════════════════

    @PostMapping("/projects/{projectId}/issues")
    // [LEARN] @PostMapping: HTTP POST 메서드 처리
    // {projectId}: URL 경로 변수 (Path Variable)
    public ResponseEntity<IssueResponse> createIssue(
            @PathVariable Long projectId,
            // [LEARN] @PathVariable: URL의 {projectId}를 파라미터로 매핑
            // /api/projects/123/issues → projectId = 123
            // C에서 URL 파싱 후 sscanf(path, "/projects/%ld/issues", &projectId) 하던 것

            @Valid @RequestBody IssueRequest request,
            // [LEARN] @RequestBody: HTTP 요청 본문(JSON)을 객체로 변환
            // {"title": "버그", "description": "..."} → IssueRequest 객체
            // @Valid: DTO의 유효성 검증 어노테이션(@NotBlank 등) 적용
            // C에서 json_parse() + 각 필드 유효성 검사를 수동으로 하던 것

            Authentication authentication
            // [LEARN] Authentication: Spring Security가 주입하는 현재 로그인 사용자 정보
            // JWT 토큰에서 추출한 userId가 principal에 저장되어 있음
            // C에서 세션/토큰에서 user_id를 추출하던 것을 프레임워크가 자동 처리
    ) {
        Long reporterId = (Long) authentication.getPrincipal();
        IssueResponse response = issueService.createIssue(projectId, reporterId, request);
        return ResponseEntity.status(HttpStatus.CREATED).body(response);
        // [LEARN] HttpStatus.CREATED: 201 상태 코드 (리소스 생성 성공)
        // REST 관례: POST 성공 시 201 Created 반환
    }

    // ═══════════════════════════════════════════════════════════════════════
    // [Order 2] 프로젝트별 이슈 목록 조회 - GET /api/projects/{projectId}/issues
    // ═══════════════════════════════════════════════════════════════════════

    @GetMapping("/projects/{projectId}/issues")
    public ResponseEntity<Page<IssueResponse>> getIssuesByProject(
            @PathVariable Long projectId,
            @RequestParam(required = false) IssueStatus status,
            // [LEARN] @RequestParam: 쿼리 파라미터 매핑
            // /api/projects/1/issues?status=OPEN → status = IssueStatus.OPEN
            // required = false: 선택적 파라미터 (없으면 null)
            // C에서 query_string을 & 기준으로 파싱하던 것

            @RequestParam(defaultValue = "0") int page,
            @RequestParam(defaultValue = "10") int size
            // [LEARN] 페이징 파라미터: ?page=0&size=10
            // defaultValue: 파라미터가 없을 때 기본값
    ) {
        Pageable pageable = PageRequest.of(page, size);
        // [LEARN] Pageable: 페이징/정렬 정보를 캡슐화한 객체
        // C에서 OFFSET = page * size, LIMIT = size 계산하던 것

        Page<IssueResponse> issues = issueService.getIssuesByProject(projectId, status, pageable);
        return ResponseEntity.ok(issues);
        // [LEARN] Page<T>는 JSON으로 변환 시 다음 구조가 됨:
        // { "content": [...], "totalElements": 100, "totalPages": 10, "number": 0, ... }
    }

    // ═══════════════════════════════════════════════════════════════════════
    // [Order 3] 이슈 단건 조회 - GET /api/issues/{id}
    // ═══════════════════════════════════════════════════════════════════════

    @GetMapping("/issues/{id}")
    public ResponseEntity<IssueResponse> getIssueById(@PathVariable Long id) {
        IssueResponse issue = issueService.getIssueById(id);
        return ResponseEntity.ok(issue);
    }

    // ═══════════════════════════════════════════════════════════════════════
    // [Order 4] 이슈 수정 - PUT /api/issues/{id}
    // ═══════════════════════════════════════════════════════════════════════

    @PutMapping("/issues/{id}")
    // [LEARN] @PutMapping: HTTP PUT 메서드 처리
    // REST 관례: PUT은 전체 리소스 교체, PATCH는 부분 수정
    // 이 구현은 부분 수정을 허용하므로 PATCH가 더 적절할 수 있음
    public ResponseEntity<IssueResponse> updateIssue(
            @PathVariable Long id,
            @Valid @RequestBody IssueUpdateRequest request
    ) {
        IssueResponse response = issueService.updateIssue(id, request);
        return ResponseEntity.ok(response);
    }

    // ═══════════════════════════════════════════════════════════════════════
    // [Order 5] 이슈 삭제 - DELETE /api/issues/{id}
    // ═══════════════════════════════════════════════════════════════════════

    @DeleteMapping("/issues/{id}")
    public ResponseEntity<Void> deleteIssue(@PathVariable Long id) {
        issueService.deleteIssue(id);
        return ResponseEntity.noContent().build();
        // [LEARN] noContent(): 204 No Content
        // REST 관례: DELETE 성공 시 본문 없이 204 반환
        // C에서 send(socket, "HTTP/1.1 204 No Content\r\n\r\n", ...) 하던 것
    }

    // ═══════════════════════════════════════════════════════════════════════
    // [Order 6] 인기 이슈 조회 - GET /api/issues/popular
    // 클론 가이드 단계: [CG-v1.2.0] Cache 적용
    // ═══════════════════════════════════════════════════════════════════════

    @GetMapping("/issues/popular")
    public ResponseEntity<List<IssueResponse>> getPopularIssues() {
        // [LEARN] 이 메서드가 호출하는 Service 메서드에 @Cacheable이 적용됨
        // 첫 호출: DB 조회 → 캐시 저장 → 응답
        // 이후 호출: 캐시에서 직접 반환 (DB 조회 생략)
        List<IssueResponse> popularIssues = issueService.getPopularIssues();
        return ResponseEntity.ok(popularIssues);
    }
}

// [Reader Notes]
// ═══════════════════════════════════════════════════════════════════════════════
// REST API 핵심 개념 요약 (C 개발자 관점):
// ───────────────────────────────────────────────────────────────────────────────
// HTTP 메서드와 CRUD 대응:
// - POST   /resources       → Create (INSERT)
// - GET    /resources       → Read All (SELECT *)
// - GET    /resources/{id}  → Read One (SELECT WHERE id = ?)
// - PUT    /resources/{id}  → Update (UPDATE)
// - DELETE /resources/{id}  → Delete (DELETE)
//
// 응답 상태 코드:
// - 200 OK: 성공
// - 201 Created: 리소스 생성 성공
// - 204 No Content: 성공, 반환 데이터 없음
// - 400 Bad Request: 잘못된 요청 (유효성 검증 실패)
// - 401 Unauthorized: 인증 필요
// - 403 Forbidden: 권한 없음
// - 404 Not Found: 리소스 없음
// - 500 Internal Server Error: 서버 오류
//
// 레이어 흐름:
// HTTP 요청 → Controller → Service → Repository → DB
//          ← DTO        ← Entity   ← SQL 결과
//
// 이 파일을 이해한 다음 볼 파일:
// ───────────────────────────────────────────────────────────────────────────────
// 1. common/security/SecurityConfig.java - 인증/인가 설정
// 2. team/controller/TeamController.java - RBAC이 적용된 Controller
// ═══════════════════════════════════════════════════════════════════════════════
