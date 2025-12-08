package com.example.training.team.domain;

import jakarta.persistence.*;
import java.time.LocalDateTime;

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * [FILE] Team 엔티티 - RBAC 구현의 핵심 엔티티
 * ─────────────────────────────────────────────────────────────────────────────────
 * - 목적: 팀 정보를 저장하는 JPA 엔티티
 * - 관련 클론 가이드 단계: [CG-v1.1.0] 1.1.1 Team 도메인 설계
 * ═══════════════════════════════════════════════════════════════════════════════
 */

// ─────────────────────────────────────────────────────────────────────────────
// [Order 1] JPA 엔티티 기본 어노테이션
// ─────────────────────────────────────────────────────────────────────────────
@Entity
// [LEARN] @Entity: 이 클래스가 DB 테이블과 매핑되는 JPA 엔티티임을 선언
// C 관점: struct Team { ... }; 를 DB 테이블로 자동 변환해주는 것

@Table(name = "teams")
// [LEARN] @Table: 매핑할 테이블 이름 지정
// 클래스명과 테이블명이 다를 때 사용 (Team → teams)
public class Team {

    // ─────────────────────────────────────────────────────────────────────────
    // [Order 2] 기본키 (Primary Key)
    // ─────────────────────────────────────────────────────────────────────────
    @Id
    @GeneratedValue(strategy = GenerationType.IDENTITY)
    // [LEARN] GenerationType.IDENTITY: DB의 AUTO_INCREMENT 사용
    // INSERT 후 DB가 생성한 ID를 가져옴 (MySQL, PostgreSQL 등)
    private Long id;

    // ─────────────────────────────────────────────────────────────────────────
    // [Order 3] 일반 컬럼
    // ─────────────────────────────────────────────────────────────────────────
    @Column(nullable = false)
    // [LEARN] nullable = false: NOT NULL 제약조건
    // DDL: name VARCHAR(255) NOT NULL
    private String name;

    @Column(nullable = false, updatable = false)
    // [LEARN] updatable = false: UPDATE 시 이 필드 변경 불가
    // 생성일시처럼 한 번 설정되면 변경되면 안 되는 필드에 사용
    private LocalDateTime createdAt;

    // ─────────────────────────────────────────────────────────────────────────
    // [Order 4] 엔티티 라이프사이클 콜백
    // ─────────────────────────────────────────────────────────────────────────
    @PrePersist
    protected void onCreate() {
        createdAt = LocalDateTime.now();
    }
    // [LEARN] @PrePersist: INSERT 직전에 자동 호출
    // C 관점: before_insert_hook() 같은 콜백 함수
    // 이렇게 하면 서비스 레이어에서 createdAt 설정을 잊어도 자동으로 채워짐

    // ─────────────────────────────────────────────────────────────────────────
    // [Order 5] 생성자
    // ─────────────────────────────────────────────────────────────────────────
    protected Team() {
    }
    // [LEARN] protected 기본 생성자: JPA 필수 (리플렉션으로 객체 생성)
    // C 관점: 프레임워크가 memset + malloc으로 객체 만드는 것과 유사

    public Team(String name) {
        this.name = name;
    }
    // [LEARN] 애플리케이션 코드에서 사용하는 생성자
    // new Team("Backend Team") 처럼 사용

    // ─────────────────────────────────────────────────────────────────────────
    // [Order 6] Getter/Setter
    // ─────────────────────────────────────────────────────────────────────────
    public Long getId() {
        return id;
    }

    public String getName() {
        return name;
    }

    public LocalDateTime getCreatedAt() {
        return createdAt;
    }

    public void setName(String name) {
        this.name = name;
    }
    // [LEARN] id에는 setter가 없음 → 외부에서 PK 변경 불가
    // 불변성(Immutability) 보장 패턴
}
