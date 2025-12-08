package com.example.training.issue.service;

import com.example.training.common.exception.ResourceNotFoundException;
import com.example.training.issue.domain.Issue;
import com.example.training.issue.domain.IssueStatus;
import com.example.training.issue.dto.IssueRequest;
import com.example.training.issue.dto.IssueResponse;
import com.example.training.issue.dto.IssueUpdateRequest;
import com.example.training.issue.repository.IssueRepository;
import com.example.training.issue.repository.ProjectRepository;
import org.springframework.cache.annotation.Cacheable;
import org.springframework.data.domain.Page;
import org.springframework.data.domain.PageRequest;
import org.springframework.data.domain.Pageable;
import org.springframework.stereotype.Service;
import org.springframework.transaction.annotation.Transactional;

import java.util.List;
import java.util.stream.Collectors;

// [FILE]
// - 목적: Issue 관련 비즈니스 로직 구현
// - 주요 역할: Controller와 Repository 사이에서 핵심 로직 처리
// - 관련 클론 가이드 단계: [CG-v1.0.0] 1.0.3 Service Layer (트랜잭션)
// - 권장 읽는 순서: Order 1(생성) → Order 2(조회) → Order 3(수정) → Order 4(삭제) → Order 5(캐시)
//
// [LEARN] C 개발자를 위한 설명:
// Service 레이어는 "비즈니스 로직을 담당하는 함수들의 모음"이다.
// C에서 여러 DB 작업을 조합하여 하나의 기능을 구현하는 함수들을
// 별도 .c 파일에 모아두는 것과 유사한 개념이다.
//
// 레이어드 아키텍처에서 Service의 역할:
// Controller (HTTP 처리) → Service (비즈니스 로직) → Repository (DB 접근)
//
// 핵심 특징:
// 1. 트랜잭션 관리: @Transactional로 ACID 보장
// 2. 비즈니스 규칙 적용: 유효성 검사, 권한 체크 등
// 3. 여러 Repository 조합: 복잡한 작업 수행

@Service
// [LEARN] @Service: 이 클래스가 비즈니스 로직 담당 Bean임을 선언
// @Component의 특수화된 형태로, 의미적으로 "서비스 레이어"임을 명시
public class IssueService {

    // [LEARN] 의존성 주입 (Dependency Injection):
    // 필요한 객체를 외부에서 주입받음 (final 필드 + 생성자)
    // C에서 함수에 컨텍스트 struct 포인터를 전달하는 것과 유사하지만,
    // Spring이 자동으로 적절한 구현체를 찾아 주입해준다
    private final IssueRepository issueRepository;
    private final ProjectRepository projectRepository;

    public IssueService(IssueRepository issueRepository, ProjectRepository projectRepository) {
        // [LEARN] 생성자 주입: Spring 권장 방식
        // 테스트 시 Mock 객체를 쉽게 주입할 수 있음
        this.issueRepository = issueRepository;
        this.projectRepository = projectRepository;
    }

    // ═══════════════════════════════════════════════════════════════════════
    // [Order 1] 이슈 생성
    // ═══════════════════════════════════════════════════════════════════════

    @Transactional
    // [LEARN] @Transactional: 이 메서드 전체를 하나의 트랜잭션으로 실행
    // - 메서드 시작 시 트랜잭션 시작 (BEGIN)
    // - 정상 완료 시 커밋 (COMMIT)
    // - 예외 발생 시 롤백 (ROLLBACK)
    //
    // C에서 직접 BEGIN/COMMIT/ROLLBACK을 호출하던 것을 어노테이션으로 선언
    // 여러 DB 작업이 "전부 성공하거나 전부 실패"하도록 보장
    public IssueResponse createIssue(Long projectId, Long reporterId, IssueRequest request) {
        // 비즈니스 규칙: 존재하는 프로젝트에만 이슈 생성 가능
        if (!projectRepository.existsById(projectId)) {
            throw new ResourceNotFoundException("Project not found: " + projectId);
        }

        // Entity 생성 및 저장
        Issue issue = new Issue(projectId, reporterId, request.getTitle(), request.getDescription());
        if (request.getAssigneeId() != null) {
            issue.setAssigneeId(request.getAssigneeId());
        }

        Issue savedIssue = issueRepository.save(issue);
        // [LEARN] save()는 INSERT 또는 UPDATE 수행
        // 새 엔티티(id가 null)면 INSERT, 기존 엔티티면 UPDATE

        return IssueResponse.from(savedIssue);
        // [LEARN] Entity → DTO 변환
        // Entity는 DB 구조를 반영, DTO는 API 응답 구조를 반영
        // 이 분리로 DB 스키마 변경이 API에 직접 영향을 주지 않음
    }

    // ═══════════════════════════════════════════════════════════════════════
    // [Order 2] 이슈 조회
    // ═══════════════════════════════════════════════════════════════════════

    @Transactional(readOnly = true)
    // [LEARN] readOnly = true: 읽기 전용 트랜잭션
    // - Hibernate의 dirty checking 비활성화 → 성능 향상
    // - 실수로 데이터 수정 시 예외 발생 → 안전장치
    public Page<IssueResponse> getIssuesByProject(Long projectId, IssueStatus status, Pageable pageable) {
        if (!projectRepository.existsById(projectId)) {
            throw new ResourceNotFoundException("Project not found: " + projectId);
        }

        Page<Issue> issues;
        if (status != null) {
            issues = issueRepository.findByProjectIdAndStatus(projectId, status, pageable);
        } else {
            issues = issueRepository.findByProjectId(projectId, pageable);
        }

        return issues.map(IssueResponse::from);
        // [LEARN] Page.map(): Page 내용물을 변환하며 페이징 메타정보 유지
        // IssueResponse::from은 메서드 레퍼런스 (C의 함수 포인터와 유사)
    }

    @Transactional
    public IssueResponse getIssueById(Long id) {
        Issue issue = issueRepository.findById(id)
                .orElseThrow(() -> new ResourceNotFoundException("Issue not found: " + id));
        // [LEARN] Optional.orElseThrow(): 값이 없으면 예외 발생
        // C에서 if (result == NULL) { return ERROR; } 패턴을 함수형으로 표현

        issue.incrementViewCount();
        issueRepository.save(issue);
        // [LEARN] 조회 시 조회수 증가 - 비즈니스 로직
        // 트랜잭션 내에서 수정되므로 ACID 보장

        return IssueResponse.from(issue);
    }

    // ═══════════════════════════════════════════════════════════════════════
    // [Order 3] 이슈 수정
    // ═══════════════════════════════════════════════════════════════════════

    @Transactional
    public IssueResponse updateIssue(Long id, IssueUpdateRequest request) {
        Issue issue = issueRepository.findById(id)
                .orElseThrow(() -> new ResourceNotFoundException("Issue not found: " + id));

        // [LEARN] 부분 업데이트 패턴: null이 아닌 필드만 수정
        // 클라이언트가 변경하고 싶은 필드만 전송할 수 있게 함
        if (request.getTitle() != null) {
            issue.setTitle(request.getTitle());
        }
        if (request.getDescription() != null) {
            issue.setDescription(request.getDescription());
        }
        if (request.getStatus() != null) {
            issue.setStatus(request.getStatus());
        }
        if (request.getAssigneeId() != null) {
            issue.setAssigneeId(request.getAssigneeId());
        }

        Issue updatedIssue = issueRepository.save(issue);
        return IssueResponse.from(updatedIssue);
    }

    // ═══════════════════════════════════════════════════════════════════════
    // [Order 4] 이슈 삭제
    // ═══════════════════════════════════════════════════════════════════════

    @Transactional
    public void deleteIssue(Long id) {
        if (!issueRepository.existsById(id)) {
            throw new ResourceNotFoundException("Issue not found: " + id);
        }
        issueRepository.deleteById(id);
    }

    // ═══════════════════════════════════════════════════════════════════════
    // [Order 5] 인기 이슈 조회 (캐싱 적용)
    // 클론 가이드 단계: [CG-v1.2.0] Cache 구현
    // ═══════════════════════════════════════════════════════════════════════

    @Cacheable(value = "popularIssues", key = "'top10'")
    // [LEARN] @Cacheable: 메서드 결과를 캐시에 저장
    // - 첫 호출: 메서드 실행 → 결과 캐시 저장 → 반환
    // - 이후 호출: 캐시에서 직접 반환 (메서드 실행 안 함)
    //
    // value = "popularIssues": 캐시 이름 (Redis 키 접두사가 됨)
    // key = "'top10'": 캐시 키 (고정 문자열)
    //
    // C에서 전역 해시맵에 결과를 저장하고 조회하던 패턴:
    // if (cache_get("top10", &result)) return result;
    // result = expensive_query();
    // cache_set("top10", result);
    // return result;
    // → 이 모든 것이 어노테이션 하나로 자동화
    @Transactional(readOnly = true)
    public List<IssueResponse> getPopularIssues() {
        List<Issue> popularIssues = issueRepository.findPopularIssues(PageRequest.of(0, 10));
        return popularIssues.stream()
                .map(IssueResponse::from)
                .collect(Collectors.toList());
        // [LEARN] Stream API: Java 함수형 프로그래밍
        // .stream() → .map() → .collect()
        // C에서 for 루프로 배열 변환하던 것을 체인 메서드로 표현
    }
}

// [Reader Notes]
// ═══════════════════════════════════════════════════════════════════════════════
// 이 파일에서 처음 등장한 개념: Service 레이어, @Transactional, @Cacheable
//
// 트랜잭션 관리 (C 개발자 관점):
// ───────────────────────────────────────────────────────────────────────────────
// [C 방식 - 수동 트랜잭션]
// mysql_query(conn, "BEGIN");
// if (insert_order(conn, order) < 0) goto rollback;
// if (update_inventory(conn, items) < 0) goto rollback;
// mysql_query(conn, "COMMIT");
// return SUCCESS;
// rollback:
// mysql_query(conn, "ROLLBACK");
// return ERROR;
//
// [Spring 방식 - 선언적 트랜잭션]
// @Transactional
// public void createOrder(Order order) {
//     orderRepository.save(order);       // 실패 시 자동 롤백
//     inventoryService.decrease(items);  // 실패 시 자동 롤백
// }  // 정상 완료 시 자동 커밋
//
// 레이어드 아키텍처 요약:
// ───────────────────────────────────────────────────────────────────────────────
// Controller: HTTP 요청/응답 처리 (어떤 URL이 어떤 기능?)
// Service: 비즈니스 로직 (어떤 규칙을 적용?)
// Repository: 데이터 접근 (어떻게 저장/조회?)
//
// 이 파일을 이해한 다음 볼 파일:
// ───────────────────────────────────────────────────────────────────────────────
// 1. issue/controller/IssueController.java - Service를 호출하는 Controller
// 2. team/service/TeamService.java - RBAC이 적용된 더 복잡한 Service
// ═══════════════════════════════════════════════════════════════════════════════
