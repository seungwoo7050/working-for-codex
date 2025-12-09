package com.example.training.team.service;

import com.example.training.common.exception.ForbiddenException;
import com.example.training.common.exception.ResourceNotFoundException;
import com.example.training.issue.domain.User;
import com.example.training.issue.repository.UserRepository;
import com.example.training.team.domain.Team;
import com.example.training.team.domain.TeamMember;
import com.example.training.team.domain.TeamRole;
import com.example.training.team.dto.*;
import com.example.training.team.repository.TeamMemberRepository;
import com.example.training.team.repository.TeamRepository;
import org.springframework.stereotype.Service;
import org.springframework.transaction.annotation.Transactional;

import java.util.List;
import java.util.stream.Collectors;

// [FILE]
// - 목적: Team 관련 비즈니스 로직 + RBAC(Role-Based Access Control) 구현
// - 주요 역할: 팀 CRUD, 멤버 관리, 역할 기반 권한 검증
// - 관련 클론 가이드 단계: [CG-v1.1.0] Team & RBAC
// - 권장 읽는 순서: Order 1(팀 생성) → Order 2(조회) → Order 3(멤버 관리) → Order 4(권한 검증)
//
// [LEARN] C 개발자를 위한 설명:
// RBAC은 "역할에 따라 수행 가능한 작업을 제한"하는 권한 모델이다.
// 이 서비스에서는 TeamRole(OWNER, MANAGER, MEMBER)에 따라 다른 권한을 부여한다.
//
// C에서 권한 검사를 구현한다면:
// enum Role { OWNER, MANAGER, MEMBER };
// int can_manage_members(Role role) { return role == OWNER || role == MANAGER; }
// if (!can_manage_members(user->role)) return FORBIDDEN;
//
// Spring에서는 이런 검사를 Service 메서드 내에서 수행하거나,
// @PreAuthorize 같은 어노테이션으로 선언적으로 처리할 수 있다.

@Service
@Transactional(readOnly = true)
// [LEARN] 클래스 레벨 @Transactional(readOnly = true):
// 모든 메서드에 기본적으로 읽기 전용 트랜잭션 적용
// 쓰기가 필요한 메서드만 @Transactional로 오버라이드
public class TeamService {

    private final TeamRepository teamRepository;
    private final TeamMemberRepository teamMemberRepository;
    private final UserRepository userRepository;

    public TeamService(TeamRepository teamRepository,
                       TeamMemberRepository teamMemberRepository,
                       UserRepository userRepository) {
        this.teamRepository = teamRepository;
        this.teamMemberRepository = teamMemberRepository;
        this.userRepository = userRepository;
    }

    // ═══════════════════════════════════════════════════════════════════════
    // [Order 1] 팀 생성 - 생성자가 자동으로 OWNER가 됨
    // ═══════════════════════════════════════════════════════════════════════

    @Transactional
    // [LEARN] 쓰기 작업이므로 readOnly = false (기본값)로 오버라이드
    public TeamResponse createTeam(Long userId, CreateTeamRequest request) {
        // 비즈니스 규칙: 사용자 존재 확인
        if (!userRepository.existsById(userId)) {
            throw new ResourceNotFoundException("User not found");
        }

        Team team = new Team(request.getName());
        team = teamRepository.save(team);

        // [LEARN] 핵심 RBAC 로직: 팀 생성자는 자동으로 OWNER 역할
        // OWNER는 모든 권한을 가짐 (멤버 추가/삭제, 역할 변경, 팀 삭제 등)
        TeamMember ownerMember = new TeamMember(team.getId(), userId, TeamRole.OWNER);
        teamMemberRepository.save(ownerMember);

        return TeamResponse.from(team);
    }

    // ═══════════════════════════════════════════════════════════════════════
    // [Order 2] 조회 - 기본적으로 팀 멤버만 조회 가능
    // ═══════════════════════════════════════════════════════════════════════

    public List<TeamResponse> getMyTeams(Long userId) {
        // [LEARN] 사용자가 속한 모든 팀 조회
        // N+1 문제 주의: 실무에서는 JOIN FETCH 또는 별도 최적화 필요
        List<TeamMember> memberships = teamMemberRepository.findByUserId(userId);
        List<Long> teamIds = memberships.stream()
                .map(TeamMember::getTeamId)
                .collect(Collectors.toList());

        return teamRepository.findAllById(teamIds).stream()
                .map(TeamResponse::from)
                .collect(Collectors.toList());
    }

    public TeamResponse getTeam(Long userId, Long teamId) {
        // [LEARN] 권한 검증: 팀 멤버가 아니면 404 반환
        // 보안 관점: 존재 여부를 숨기기 위해 403 대신 404 사용
        assertTeamMember(userId, teamId);
        Team team = teamRepository.findById(teamId)
                .orElseThrow(() -> new ResourceNotFoundException("Team not found"));
        return TeamResponse.from(team);
    }

    public List<TeamMemberResponse> getTeamMembers(Long userId, Long teamId) {
        assertTeamMember(userId, teamId);
        return teamMemberRepository.findByTeamId(teamId).stream()
                .map(TeamMemberResponse::from)
                .collect(Collectors.toList());
    }

    // ═══════════════════════════════════════════════════════════════════════
    // [Order 3] 멤버 관리 - OWNER 또는 MANAGER만 가능
    // ═══════════════════════════════════════════════════════════════════════

    @Transactional
    public TeamMemberResponse addTeamMember(Long userId, Long teamId, AddTeamMemberRequest request) {
        // [LEARN] RBAC 핵심: 권한 검증을 먼저 수행
        // assertCanManageMembers: OWNER 또는 MANAGER만 통과
        assertCanManageMembers(userId, teamId);

        // 대상 사용자 존재 확인
        if (!userRepository.existsById(request.getUserId())) {
            throw new ResourceNotFoundException("User not found");
        }

        // 중복 멤버 체크
        if (teamMemberRepository.existsByTeamIdAndUserId(teamId, request.getUserId())) {
            throw new IllegalArgumentException("User is already a team member");
        }

        TeamMember member = new TeamMember(teamId, request.getUserId(), request.getRole());
        member = teamMemberRepository.save(member);
        return TeamMemberResponse.from(member);
    }

    @Transactional
    public TeamMemberResponse updateTeamMemberRole(Long userId, Long teamId, Long memberId,
                                                     UpdateTeamMemberRoleRequest request) {
        assertCanManageMembers(userId, teamId);

        TeamMember member = teamMemberRepository.findById(memberId)
                .orElseThrow(() -> new ResourceNotFoundException("Team member not found"));

        if (!member.getTeamId().equals(teamId)) {
            throw new ResourceNotFoundException("Team member not found in this team");
        }

        // [LEARN] 비즈니스 규칙: 마지막 OWNER는 역할 변경 불가
        // 팀에는 최소 1명의 OWNER가 있어야 함
        if (member.getRole() == TeamRole.OWNER && request.getRole() != TeamRole.OWNER) {
            int ownerCount = teamMemberRepository.countByTeamIdAndRole(teamId, TeamRole.OWNER);
            if (ownerCount <= 1) {
                throw new IllegalArgumentException("Cannot remove the last owner");
            }
        }

        member.setRole(request.getRole());
        member = teamMemberRepository.save(member);
        return TeamMemberResponse.from(member);
    }

    @Transactional
    public void removeTeamMember(Long userId, Long teamId, Long memberId) {
        assertCanManageMembers(userId, teamId);

        TeamMember member = teamMemberRepository.findById(memberId)
                .orElseThrow(() -> new ResourceNotFoundException("Team member not found"));

        if (!member.getTeamId().equals(teamId)) {
            throw new ResourceNotFoundException("Team member not found in this team");
        }

        // [LEARN] 마지막 OWNER 삭제 방지
        if (member.getRole() == TeamRole.OWNER) {
            int ownerCount = teamMemberRepository.countByTeamIdAndRole(teamId, TeamRole.OWNER);
            if (ownerCount <= 1) {
                throw new IllegalArgumentException("Cannot remove the last owner");
            }
        }

        teamMemberRepository.delete(member);
    }

    // ═══════════════════════════════════════════════════════════════════════
    // [Order 4] 권한 검증 헬퍼 메서드
    // ═══════════════════════════════════════════════════════════════════════

    // [LEARN] 팀 멤버 여부 확인
    // 팀에 속하지 않은 사용자는 팀 정보에 접근 불가
    private void assertTeamMember(Long userId, Long teamId) {
        if (!teamMemberRepository.existsByTeamIdAndUserId(teamId, userId)) {
            // [LEARN] 보안: 팀 존재 여부를 숨기기 위해 404 사용
            // "이 팀은 없거나, 당신은 멤버가 아닙니다"
            throw new ResourceNotFoundException("Team not found");
        }
    }

    // [LEARN] 멤버 관리 권한 확인
    // OWNER 또는 MANAGER만 다른 멤버를 추가/삭제/역할변경 가능
    private void assertCanManageMembers(Long userId, Long teamId) {
        TeamMember member = teamMemberRepository.findByTeamIdAndUserId(teamId, userId)
                .orElseThrow(() -> new ResourceNotFoundException("Team not found"));

        if (member.getRole() != TeamRole.OWNER && member.getRole() != TeamRole.MANAGER) {
            // [LEARN] 팀 멤버지만 권한이 없는 경우 403 Forbidden
            throw new ForbiddenException("Only OWNER or MANAGER can manage team members");
        }
    }
}

// [Reader Notes]
// ═══════════════════════════════════════════════════════════════════════════════
// 이 파일에서 처음 등장한 개념: RBAC (Role-Based Access Control)
//
// RBAC 핵심 개념 (C 개발자 관점):
// ───────────────────────────────────────────────────────────────────────────────
// 1. 역할(Role) 계층:
//    OWNER > MANAGER > MEMBER
//    - OWNER: 모든 권한 (팀 삭제, 멤버 관리, 역할 변경)
//    - MANAGER: 멤버 관리 (추가, 삭제, 역할 변경 - OWNER 제외)
//    - MEMBER: 읽기 전용 (팀 정보 조회)
//
// 2. 권한 검증 패턴:
//    [C 방식]
//    if (get_user_role(user_id, team_id) < REQUIRED_ROLE) {
//        return ERROR_FORBIDDEN;
//    }
//
//    [Spring 방식 - 명시적]
//    assertCanManageMembers(userId, teamId);  // 실패 시 예외
//
//    [Spring 방식 - 선언적] (이 코드에는 없지만)
//    @PreAuthorize("hasRole('OWNER') or hasRole('MANAGER')")
//
// 3. 보안 원칙:
//    - 존재 여부 숨기기: 팀이 있지만 권한 없으면 404 반환
//    - 최소 권한 원칙: 필요한 권한만 부여
//    - 마지막 OWNER 보호: 팀에 항상 관리자가 있도록 보장
//
// 이 파일을 이해한 다음 볼 파일:
// ───────────────────────────────────────────────────────────────────────────────
// 1. team/domain/TeamRole.java - 역할 enum 정의
// 2. stats/service/StatsService.java - 배치/통계 서비스
// ═══════════════════════════════════════════════════════════════════════════════
