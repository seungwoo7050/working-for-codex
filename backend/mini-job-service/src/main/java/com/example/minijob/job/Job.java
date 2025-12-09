package com.example.minijob.job;

import java.time.Instant;

// [FILE]
// - 목적: Job 도메인의 핵심 모델을 정의한다.
// - 주요 역할: 작업 식별자, 제목, 상태, 생성 시각을 보유한다.
// - 관련 토이 버전: [BE-v0.2]
// - 권장 읽는 순서: 필드 정의 -> 생성자 -> 상태 전환 메서드.
//
// [LEARN] 간단한 엔티티 클래스를 정의하고 향후 JPA 엔티티로 확장할 수 있는 구조를 익힌다.
public class Job {
    private Long id;
    private String title;
    private String status;
    private Instant createdAt;

    public Job(Long id, String title, String status, Instant createdAt) {
        this.id = id;
        this.title = title;
        this.status = status;
        this.createdAt = createdAt;
    }

    public Long getId() {
        return id;
    }

    public void setId(Long id) {
        this.id = id;
    }

    public String getTitle() {
        return title;
    }

    public void setTitle(String title) {
        this.title = title;
    }

    public String getStatus() {
        return status;
    }

    public void setStatus(String status) {
        this.status = status;
    }

    public Instant getCreatedAt() {
        return createdAt;
    }

    public void setCreatedAt(Instant createdAt) {
        this.createdAt = createdAt;
    }
}
