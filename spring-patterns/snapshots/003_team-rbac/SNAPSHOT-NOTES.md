# 스냅샷 003: Team & RBAC (v1.1.0)

## 개요
- **대응 버전**: v1.1.0
- **패치 범위**: 1.1.1 ~ 1.1.5
- **빌드 가능**: ✅ 예
- **테스트 가능**: ✅ 예

## 포함된 기능
- v1.0.0의 모든 기능 (Layered CRUD)
- Team CRUD
- TeamMember 관리
- RBAC (Role-Based Access Control)
  - OWNER: 팀 전체 관리 권한
  - MANAGER: 팀원 관리 권한
  - MEMBER: 기본 멤버 권한
- WorkspaceItem CRUD

## 새로 추가된 컴포넌트
### Entity
- Team, TeamMember, TeamRole (enum), WorkspaceItem

### Repository
- TeamRepository, TeamMemberRepository, WorkspaceItemRepository

### Service
- TeamService, WorkspaceItemService

### Controller
- TeamController, WorkspaceItemController

### DTO
- CreateTeamRequest, TeamResponse
- AddTeamMemberRequest, TeamMemberResponse, UpdateTeamMemberRoleRequest
- CreateWorkspaceItemRequest, UpdateWorkspaceItemRequest, WorkspaceItemResponse

### Exception
- ForbiddenException (RBAC 권한 검사용)

## 빌드 방법
```bash
cd snapshots/003_team-rbac
./gradlew build
```

## 테스트 방법
```bash
./gradlew test
```

## 실행 방법
```bash
./gradlew bootRun
```

## API 엔드포인트

### Team API
- `POST /api/teams` - 팀 생성 (생성자는 자동으로 OWNER)
- `GET /api/teams` - 내 팀 목록 조회
- `GET /api/teams/{id}` - 팀 상세 조회
- `GET /api/teams/{id}/members` - 팀원 목록 조회
- `POST /api/teams/{id}/members` - 팀원 추가 (OWNER/MANAGER만)
- `PATCH /api/teams/{id}/members/{memberId}` - 팀원 역할 변경 (OWNER/MANAGER만)
- `DELETE /api/teams/{id}/members/{memberId}` - 팀원 제거 (OWNER/MANAGER만)

### WorkspaceItem API
- `POST /api/teams/{teamId}/items` - 워크스페이스 아이템 생성
- `GET /api/teams/{teamId}/items` - 팀의 아이템 목록 조회
- `GET /api/items/{id}` - 아이템 상세 조회
- `PUT /api/items/{id}` - 아이템 수정
- `DELETE /api/items/{id}` - 아이템 삭제

## RBAC 권한 체계
| 권한 | OWNER | MANAGER | MEMBER |
|------|-------|---------|--------|
| 팀 조회 | ✅ | ✅ | ✅ |
| 팀원 목록 조회 | ✅ | ✅ | ✅ |
| 팀원 추가 | ✅ | ✅ | ❌ |
| 팀원 역할 변경 | ✅ | ✅ | ❌ |
| 팀원 제거 | ✅ | ✅ | ❌ |
| 아이템 CRUD | ✅ | ✅ | ✅ |
