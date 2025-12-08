package com.example.training.issue.dto;

import com.example.training.issue.domain.Issue;
import com.example.training.issue.domain.IssueStatus;

import java.time.LocalDateTime;

// [FILE]
// - 목적: Issue 조회 응답용 DTO (Data Transfer Object)
// - 주요 역할: API 응답 형식 정의, Entity → JSON 변환 구조
// - 관련 클론 가이드 단계: [CG-v1.0.0] 1.0.5 DTO & Mapper
// - 권장 읽는 순서: from() 팩토리 메서드 → 필드 → getter
//
// [LEARN] C 개발자를 위한 설명:
// DTO는 "API 응답 전용 데이터 구조체"다.
//
// Entity vs DTO 분리 이유:
// 1. Entity: DB 테이블 구조를 반영 (JPA 관리)
// 2. DTO: API 응답 구조를 반영 (클라이언트 요구사항)
//
// C 관점:
// // Entity (DB 매핑)
// typedef struct {
//     long id;
//     char password_hash[64];  // DB에는 저장하지만
//     ...
// } IssueEntity;
//
// // DTO (API 응답)
// typedef struct {
//     long id;
//     // password_hash 제외!  // 클라이언트에게 노출 안 함
//     ...
// } IssueResponse;
//
// 분리의 장점:
// - 보안: 민감 정보(비밀번호 등) 노출 방지
// - 유연성: DB 스키마 변경이 API 스펙에 영향 안 줌
// - 가독성: API 문서와 코드가 일치
public class IssueResponse {
    private Long id;
    private Long projectId;
    private Long reporterId;
    private Long assigneeId;
    private String title;
    private String description;
    private IssueStatus status;
    private LocalDateTime createdAt;
    private LocalDateTime updatedAt;

    public IssueResponse(Long id, Long projectId, Long reporterId, Long assigneeId,
                         String title, String description, IssueStatus status,
                         LocalDateTime createdAt, LocalDateTime updatedAt) {
        this.id = id;
        this.projectId = projectId;
        this.reporterId = reporterId;
        this.assigneeId = assigneeId;
        this.title = title;
        this.description = description;
        this.status = status;
        this.createdAt = createdAt;
        this.updatedAt = updatedAt;
    }

    // [LEARN] 정적 팩토리 메서드: Entity → DTO 변환
    // 생성자 대신 from()을 사용하면:
    // 1. 의미가 명확함 (IssueResponse.from(issue) vs new IssueResponse(...))
    // 2. 변환 로직을 한 곳에서 관리
    public static IssueResponse from(Issue issue) {
        return new IssueResponse(
                issue.getId(),
                issue.getProjectId(),
                issue.getReporterId(),
                issue.getAssigneeId(),
                issue.getTitle(),
                issue.getDescription(),
                issue.getStatus(),
                issue.getCreatedAt(),
                issue.getUpdatedAt()
        );
    }

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
}
