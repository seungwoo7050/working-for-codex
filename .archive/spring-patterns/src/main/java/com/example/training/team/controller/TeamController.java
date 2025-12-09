package com.example.training.team.controller;

import com.example.training.team.dto.*;
import com.example.training.team.service.TeamService;
import jakarta.validation.Valid;
import org.springframework.http.HttpStatus;
import org.springframework.http.ResponseEntity;
import org.springframework.security.core.Authentication;
import org.springframework.web.bind.annotation.*;

import java.util.List;

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * [FILE] TeamController - RBAC 기반 팀 관리 API
 * ─────────────────────────────────────────────────────────────────────────────────
 * - 목적: 팀 CRUD + 멤버 관리 REST API 제공
 * - 관련 클론 가이드 단계: [CG-v1.1.0] 1.1.3 팀 관리 API
 * 
 * [LEARN] RESTful 리소스 설계:
 * - /api/teams: 팀 컬렉션
 * - /api/teams/{id}: 개별 팀
 * - /api/teams/{id}/members: 팀 멤버 (중첩 리소스)
 * - /api/teams/{id}/members/{memberId}: 개별 멤버
 * 
 * HTTP 메서드:
 * - POST: 생성 (Create)
 * - GET: 조회 (Read)
 * - PATCH: 부분 수정 (Update)
 * - DELETE: 삭제 (Delete)
 * ═══════════════════════════════════════════════════════════════════════════════
 */
@RestController
@RequestMapping("/api/teams")
public class TeamController {

    private final TeamService teamService;

    public TeamController(TeamService teamService) {
        this.teamService = teamService;
    }

    // ─────────────────────────────────────────────────────────────────────────
    // [Order 1] 팀 생성
    // ─────────────────────────────────────────────────────────────────────────
    @PostMapping
    public ResponseEntity<TeamResponse> createTeam(
            Authentication authentication,
            @Valid @RequestBody CreateTeamRequest request
    ) {
        Long userId = (Long) authentication.getPrincipal();
        // [LEARN] Authentication.getPrincipal():
        // JwtAuthenticationFilter에서 설정한 사용자 ID 추출
        
        TeamResponse response = teamService.createTeam(userId, request);
        return ResponseEntity.status(HttpStatus.CREATED).body(response);
        // [LEARN] 201 Created: 리소스 생성 성공 표준 응답
    }

    // ─────────────────────────────────────────────────────────────────────────
    // [Order 2] 내 팀 목록 조회
    // ─────────────────────────────────────────────────────────────────────────
    @GetMapping
    public ResponseEntity<List<TeamResponse>> getMyTeams(Authentication authentication) {
        Long userId = (Long) authentication.getPrincipal();
        List<TeamResponse> teams = teamService.getMyTeams(userId);
        return ResponseEntity.ok(teams);
    }
    // [LEARN] 사용자가 속한 모든 팀 조회
    // RBAC: 인증된 사용자만 자신의 팀 조회 가능

    // ─────────────────────────────────────────────────────────────────────────
    // [Order 3] 팀 상세 조회
    // ─────────────────────────────────────────────────────────────────────────
    @GetMapping("/{id}")
    public ResponseEntity<TeamResponse> getTeam(
            Authentication authentication,
            @PathVariable Long id
    ) {
        Long userId = (Long) authentication.getPrincipal();
        TeamResponse team = teamService.getTeam(userId, id);
        // [LEARN] TeamService에서 멤버십 검증 수행
        // 팀 멤버가 아니면 403 또는 404 응답
        return ResponseEntity.ok(team);
    }

    // ─────────────────────────────────────────────────────────────────────────
    // [Order 4] 팀 멤버 관리 (중첩 리소스)
    // ─────────────────────────────────────────────────────────────────────────
    @GetMapping("/{id}/members")
    public ResponseEntity<List<TeamMemberResponse>> getTeamMembers(
            Authentication authentication,
            @PathVariable Long id
    ) {
        Long userId = (Long) authentication.getPrincipal();
        List<TeamMemberResponse> members = teamService.getTeamMembers(userId, id);
        return ResponseEntity.ok(members);
    }
    // [LEARN] 중첩 리소스 패턴: /teams/{id}/members
    // 팀에 속한 멤버들을 조회 (팀 컨텍스트 내에서)

    @PostMapping("/{id}/members")
    public ResponseEntity<TeamMemberResponse> addTeamMember(
            Authentication authentication,
            @PathVariable Long id,
            @Valid @RequestBody AddTeamMemberRequest request
    ) {
        Long userId = (Long) authentication.getPrincipal();
        TeamMemberResponse member = teamService.addTeamMember(userId, id, request);
        // [LEARN] RBAC 검증: OWNER 또는 MANAGER만 멤버 추가 가능
        return ResponseEntity.status(HttpStatus.CREATED).body(member);
    }

    @PatchMapping("/{id}/members/{memberId}")
    public ResponseEntity<TeamMemberResponse> updateTeamMemberRole(
            Authentication authentication,
            @PathVariable Long id,
            @PathVariable Long memberId,
            @Valid @RequestBody UpdateTeamMemberRoleRequest request
    ) {
        Long userId = (Long) authentication.getPrincipal();
        TeamMemberResponse member = teamService.updateTeamMemberRole(userId, id, memberId, request);
        // [LEARN] @PatchMapping: 부분 수정 (역할만 변경)
        // PUT vs PATCH:
        // - PUT: 전체 리소스 교체
        // - PATCH: 일부 필드만 수정
        return ResponseEntity.ok(member);
    }

    @DeleteMapping("/{id}/members/{memberId}")
    public ResponseEntity<Void> removeTeamMember(
            Authentication authentication,
            @PathVariable Long id,
            @PathVariable Long memberId
    ) {
        Long userId = (Long) authentication.getPrincipal();
        teamService.removeTeamMember(userId, id, memberId);
        return ResponseEntity.noContent().build();
        // [LEARN] 204 No Content: 삭제 성공, 응답 본문 없음
    }
}
