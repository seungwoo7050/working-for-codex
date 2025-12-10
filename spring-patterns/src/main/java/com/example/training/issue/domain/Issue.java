package com.example.training.issue.domain;

import jakarta.persistence.*;
import java.time.LocalDateTime;

// [FILE]
// - 목적: Issue(이슈/티켓) 도메인 엔티티 정의
// - 주요 역할: DB의 issues 테이블과 1:1 매핑되는 JPA Entity
// - 관련 클론 가이드 단계: [CG-v1.0.0] 1.0.1 JPA Entity 정의
// - 권장 읽는 순서: Order 1(필드) → Order 2(생명주기) → Order 3(생성자) → Order 4(getter/setter)
//
// [LEARN] C 개발자를 위한 설명:
// JPA Entity는 "DB 테이블을 Java 클래스로 표현"한 것이다.
// C에서 DB 작업을 할 때 struct로 레코드를 정의하고,
// SQL 결과를 수동으로 struct에 매핑하던 작업을 ORM이 자동화한다.
//
// 핵심 차이점:
// - C: SELECT * FROM issues → while(fetch) { issue.id = row[0]; ... }
// - JPA: issueRepository.findById(id) → 자동으로 Issue 객체 반환
//
// Entity는 "영속성 컨텍스트"에서 관리되며, 필드 변경 시
// 트랜잭션 커밋 시점에 자동으로 UPDATE SQL이 실행된다.

@Entity
// [LEARN] @Entity: 이 클래스가 JPA Entity임을 선언
// JPA는 이 어노테이션을 보고 DB 테이블과 매핑을 설정한다.
// C의 ORM 라이브러리에서 테이블 메타데이터를 정의하는 것과 유사.

@Table(name = "issues")
// [LEARN] @Table: 매핑할 테이블명 지정 (생략 시 클래스명 사용)
public class Issue {

    // ═══════════════════════════════════════════════════════════════════════
    // [Order 1] 필드 정의 - DB 컬럼과 1:1 매핑
    // ═══════════════════════════════════════════════════════════════════════

    @Id
    // [LEARN] @Id: 이 필드가 Primary Key임을 선언
    // C에서 struct { int id; ... }의 id가 PK라고 "선언"하는 것과 동일
    @GeneratedValue(strategy = GenerationType.IDENTITY)
    // [LEARN] @GeneratedValue: PK 자동 생성 전략 지정
    // IDENTITY = DB의 AUTO_INCREMENT 사용 (MySQL, PostgreSQL의 SERIAL)
    private Long id;

    @Column(nullable = false)
    // [LEARN] @Column: 컬럼 속성 지정 (nullable, length, unique 등)
    // nullable = false는 NOT NULL 제약조건과 동일
    private Long projectId;

    @Column(nullable = false)
    private Long reporterId;

    private Long assigneeId;
    // [LEARN] 어노테이션이 없는 필드도 자동으로 컬럼이 된다
    // nullable 제약이 없으므로 NULL 허용

    @Column(nullable = false)
    private String title;

    @Column(length = 2000)
    private String description;

    @Enumerated(EnumType.STRING)
    // [LEARN] @Enumerated: Java enum을 DB에 저장하는 방식 지정
    // EnumType.STRING: enum의 이름(OPEN, IN_PROGRESS 등)을 문자열로 저장
    // EnumType.ORDINAL: enum의 순서(0, 1, 2...)를 정수로 저장 (비권장)
    @Column(nullable = false)
    private IssueStatus status;

    @Column(nullable = false, updatable = false)
    // [LEARN] updatable = false: INSERT 후 UPDATE로 변경 불가
    private LocalDateTime createdAt;

    @Column(nullable = false)
    private LocalDateTime updatedAt;

    @Column(nullable = false)
    private Integer viewCount = 0;

    // ═══════════════════════════════════════════════════════════════════════
    // [Order 2] 엔티티 생명주기 콜백
    // ═══════════════════════════════════════════════════════════════════════

    @PrePersist
    // [LEARN] @PrePersist: INSERT 직전에 자동 호출되는 콜백
    // C에서 save_to_db() 함수 내에서 timestamp = time(NULL) 하던 것을
    // JPA가 자동으로 호출해준다.
    protected void onCreate() {
        createdAt = LocalDateTime.now();
        updatedAt = LocalDateTime.now();
        if (viewCount == null) {
            viewCount = 0;
        }
    }

    @PreUpdate
    // [LEARN] @PreUpdate: UPDATE 직전에 자동 호출
    protected void onUpdate() {
        updatedAt = LocalDateTime.now();
    }

    // ═══════════════════════════════════════════════════════════════════════
    // [Order 3] 생성자
    // ═══════════════════════════════════════════════════════════════════════

    protected Issue() {
        // [LEARN] JPA는 리플렉션으로 객체를 생성하므로 기본 생성자 필수
        // protected로 선언하여 외부에서 직접 호출은 막고,
        // JPA 프레임워크만 사용하도록 제한
    }

    public Issue(Long projectId, Long reporterId, String title, String description) {
        // [LEARN] 비즈니스 로직에서 사용할 생성자
        // 필수 필드를 파라미터로 받아 객체 생성
        this.projectId = projectId;
        this.reporterId = reporterId;
        this.title = title;
        this.description = description;
        this.status = IssueStatus.OPEN;
        // [LEARN] 새 이슈는 항상 OPEN 상태로 시작 (비즈니스 규칙)
    }

    // ═══════════════════════════════════════════════════════════════════════
    // [Order 4] Getter/Setter
    // ═══════════════════════════════════════════════════════════════════════
    // [LEARN] Java의 캡슐화 관례: 필드는 private, 접근은 getter/setter
    // C의 struct에서 필드를 직접 접근하던 것과 달리,
    // 메서드를 통해 접근함으로써 유효성 검사나 부가 로직 추가 가능

    public Long getId() {
        return id;
    }

    public Long getProjectId() {
        return projectId;
    }

    public Long getReporterId() {
        return reporterId;
    }

    public Long getAssigneeId() {
        return assigneeId;
    }

    public String getTitle() {
        return title;
    }

    public String getDescription() {
        return description;
    }

    public IssueStatus getStatus() {
        return status;
    }

    public LocalDateTime getCreatedAt() {
        return createdAt;
    }

    public LocalDateTime getUpdatedAt() {
        return updatedAt;
    }

    public void setTitle(String title) {
        this.title = title;
    }

    public void setDescription(String description) {
        this.description = description;
    }

    public void setStatus(IssueStatus status) {
        this.status = status;
    }

    public void setAssigneeId(Long assigneeId) {
        this.assigneeId = assigneeId;
    }

    public Integer getViewCount() {
        return viewCount;
    }

    public void incrementViewCount() {
        this.viewCount++;
    }
}

// [Reader Notes]
// ═══════════════════════════════════════════════════════════════════════════════
// 이 파일에서 처음 등장한 개념: JPA Entity, ORM 매핑
//
// JPA/ORM 핵심 개념 (C 개발자 관점):
// ───────────────────────────────────────────────────────────────────────────────
// 1. Entity = DB 레코드의 객체 표현
//    - C: struct Issue { int id; char* title; ... } + 수동 SQL 매핑
//    - JPA: @Entity class Issue { ... } + 자동 SQL 생성
//
// 2. 영속성 컨텍스트 (Persistence Context):
//    - Entity 객체는 "관리 상태"에서 변경 추적됨
//    - 필드 수정 → 트랜잭션 커밋 시 자동 UPDATE
//    - C에서 dirty flag를 수동 관리하던 것을 자동화
//
// 3. 관계 매핑 (이 파일에서는 ID만 저장):
//    - projectId, reporterId는 외래키 ID만 저장 (단순한 방식)
//    - 복잡한 경우 @ManyToOne, @OneToMany로 객체 참조 가능
//
// 4. 어노테이션 = DDL 메타데이터:
//    - @Column(nullable = false) → NOT NULL
//    - @Column(length = 2000) → VARCHAR(2000)
//    - JPA가 스키마 자동 생성 시 이 정보 사용
//
// 이 파일을 이해한 다음 볼 파일:
// ───────────────────────────────────────────────────────────────────────────────
// 1. issue/repository/IssueRepository.java - Entity를 DB에 저장/조회하는 인터페이스
// 2. issue/service/IssueService.java - 비즈니스 로직 구현
// ═══════════════════════════════════════════════════════════════════════════════
