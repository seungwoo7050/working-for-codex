# 스냅샷 002: Layered CRUD (v1.0.0)

## 개요
- **대응 버전**: v1.0.0
- **패치 범위**: 1.0.1 ~ 1.0.7
- **빌드 가능**: ✅ 예
- **테스트 가능**: ✅ 예

## 포함된 기능
- v0.1.0의 모든 기능 (Bootstrap)
- Issue CRUD (레이어드 아키텍처)
- User 회원가입
- Project CRUD
- Comment CRUD
- JWT 기반 인증
- 페이징 & 정렬

## 새로 추가된 컴포넌트
### Entity
- Issue, User, Project, Comment, IssueStatus

### Repository
- IssueRepository, UserRepository, ProjectRepository, CommentRepository

### Service
- IssueService, UserService, ProjectService, CommentService, AuthService

### Controller
- IssueController, UserController, ProjectController, CommentController, AuthController

### Security
- JwtUtil, JwtAuthenticationFilter, SecurityConfig

## 빌드 방법
```bash
cd snapshots/002_layered-crud
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

## API 테스트
```bash
# 회원가입
curl -X POST http://localhost:8080/api/users \
  -H "Content-Type: application/json" \
  -d '{"email":"test@example.com","password":"password123","nickname":"tester"}'

# 로그인
curl -X POST http://localhost:8080/api/auth/login \
  -H "Content-Type: application/json" \
  -d '{"email":"test@example.com","password":"password123"}'

# 프로젝트 생성 (토큰 필요)
curl -X POST http://localhost:8080/api/projects \
  -H "Content-Type: application/json" \
  -H "Authorization: Bearer <TOKEN>" \
  -d '{"name":"Test Project","description":"A test project"}'
```
