package com.example.training.issue.repository;

import com.example.training.issue.domain.Issue;
import com.example.training.issue.domain.IssueStatus;
import org.springframework.data.domain.Page;
import org.springframework.data.domain.Pageable;
import org.springframework.data.jpa.repository.JpaRepository;
import org.springframework.data.jpa.repository.Query;
import org.springframework.stereotype.Repository;

import java.time.LocalDateTime;
import java.util.List;

// [FILE]
// - 목적: Issue 엔티티의 데이터 접근 계층 (Repository)
// - 주요 역할: DB CRUD 작업을 위한 인터페이스 정의
// - 관련 클론 가이드 단계: [CG-v1.0.0] 1.0.2 Repository 인터페이스
// - 권장 읽는 순서: Order 1(기본 CRUD) → Order 2(커스텀 쿼리)
//
// [LEARN] C 개발자를 위한 설명:
// C에서 DB 작업을 할 때는 SQL 문자열을 직접 작성하고,
// prepare → bind → execute → fetch 사이클을 수동으로 구현했다.
//
// Spring Data JPA는 "인터페이스만 정의하면 구현체를 자동 생성"한다:
// - 메서드 이름 규칙을 분석하여 SQL 자동 생성
// - findByProjectId → SELECT * FROM issues WHERE project_id = ?
// - countByCreatedAtBetween → SELECT COUNT(*) FROM issues WHERE created_at BETWEEN ? AND ?
//
// 이것이 "Repository 패턴"의 핵심: 데이터 접근 로직을 추상화

@Repository
// [LEARN] @Repository: 이 인터페이스가 데이터 접근 계층임을 선언
// Spring이 이를 감지하고 프록시 구현체를 자동 생성한다
public interface IssueRepository extends JpaRepository<Issue, Long> {
    // [LEARN] JpaRepository<Entity타입, PK타입>을 상속하면
    // 기본 CRUD 메서드가 자동으로 제공된다:
    // - save(entity) → INSERT 또는 UPDATE
    // - findById(id) → SELECT WHERE id = ?
    // - findAll() → SELECT *
    // - deleteById(id) → DELETE WHERE id = ?
    // - count() → SELECT COUNT(*)
    //
    // C에서 각각의 SQL을 직접 작성하던 것이 상속만으로 해결된다

    // ═══════════════════════════════════════════════════════════════════════
    // [Order 1] 메서드 이름 기반 쿼리 (Query Method)
    // ═══════════════════════════════════════════════════════════════════════

    // [LEARN] 메서드 이름 규칙: findBy + 필드명 + 조건
    // Spring Data가 메서드 이름을 파싱하여 SQL 자동 생성
    Page<Issue> findByProjectId(Long projectId, Pageable pageable);
    // → SELECT * FROM issues WHERE project_id = ? ORDER BY ... LIMIT ... OFFSET ...

    Page<Issue> findByProjectIdAndStatus(Long projectId, IssueStatus status, Pageable pageable);
    // → SELECT * FROM issues WHERE project_id = ? AND status = ? ...

    // ═══════════════════════════════════════════════════════════════════════
    // [Order 2] 통계용 쿼리 메서드
    // 클론 가이드 단계: [CG-v1.2.0] Batch, Stats 구현 시 사용
    // ═══════════════════════════════════════════════════════════════════════

    // [LEARN] count로 시작하면 COUNT(*) 쿼리 생성
    int countByCreatedAtBetween(LocalDateTime start, LocalDateTime end);
    // → SELECT COUNT(*) FROM issues WHERE created_at BETWEEN ? AND ?

    // ═══════════════════════════════════════════════════════════════════════
    // [Order 3] @Query: JPQL 직접 작성
    // ═══════════════════════════════════════════════════════════════════════

    @Query("SELECT COUNT(i) FROM Issue i WHERE i.status IN ('RESOLVED', 'CLOSED') " +
           "AND i.updatedAt BETWEEN :start AND :end")
    // [LEARN] @Query: 메서드 이름으로 표현하기 어려운 복잡한 쿼리는 직접 작성
    // JPQL(Java Persistence Query Language): SQL과 유사하지만 테이블 대신 Entity 사용
    // - FROM Issue i → 테이블명(issues)이 아닌 클래스명(Issue) 사용
    // - :start, :end → 메서드 파라미터와 바인딩 (SQL Injection 방지)
    int countResolvedBetween(LocalDateTime start, LocalDateTime end);

    @Query("SELECT i FROM Issue i ORDER BY (i.viewCount + " +
           "(SELECT COUNT(c) FROM Comment c WHERE c.issueId = i.id)) DESC")
    // [LEARN] 서브쿼리와 연산을 포함한 복잡한 정렬
    // 인기 이슈 = 조회수 + 댓글수 기준 내림차순
    // 클론 가이드 단계: [CG-v1.2.0] Cache 적용 대상
    List<Issue> findPopularIssues(Pageable pageable);
}

// [Reader Notes]
// ═══════════════════════════════════════════════════════════════════════════════
// 이 파일에서 처음 등장한 개념: Spring Data Repository, Query Method
//
// Repository 패턴 (C 개발자 관점):
// ───────────────────────────────────────────────────────────────────────────────
// 1. 전통적 C 방식 vs Repository 패턴:
//
//    [C 방식]
//    char* sql = "SELECT * FROM issues WHERE project_id = ?";
//    stmt = mysql_prepare(conn, sql);
//    mysql_bind_param(stmt, 1, &project_id);
//    mysql_execute(stmt);
//    while (row = mysql_fetch(stmt)) {
//        issue->id = atoi(row[0]);
//        issue->title = strdup(row[1]);
//        ...
//    }
//
//    [Repository 패턴]
//    Page<Issue> issues = issueRepository.findByProjectId(projectId, pageable);
//    // 끝. SQL 작성, 바인딩, 매핑 모두 자동
//
// 2. Query Method 명명 규칙:
//    - findBy, countBy, existsBy, deleteBy + 필드명
//    - And, Or로 조건 연결
//    - Between, LessThan, GreaterThan, Like, In, OrderBy 등
//    - 예: findByStatusAndCreatedAtAfterOrderByIdDesc
//
// 3. Pageable: 페이징/정렬 정보 캡슐화
//    - Page<T> 반환 시 전체 개수, 페이지 수 등 메타정보도 포함
//    - C에서 LIMIT, OFFSET, ORDER BY를 수동 처리하던 것을 추상화
//
// 이 파일을 이해한 다음 볼 파일:
// ───────────────────────────────────────────────────────────────────────────────
// 1. issue/service/IssueService.java - Repository를 사용하는 비즈니스 로직
// ═══════════════════════════════════════════════════════════════════════════════
