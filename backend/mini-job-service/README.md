> ⚠️ 이 리포지토리는 **학습용 토이 프로젝트**입니다.
>   실서비스 운영/보안/장애 대응을 전제로 설계되지 않았습니다.

# mini-job-service
Spring Boot 기반의 간단한 잡 관리 API입니다. BE-v0.1 ~ BE-v0.8 단계를 통해 헬스체크, 인메모리 CRUD, JPA/PostgreSQL, JWT 인증을 순차적으로 학습합니다.

## Version Roadmap
- BE-v0.1: 헬스체크 엔드포인트
- BE-v0.2: Job 도메인 + 인메모리 리포지토리
- BE-v0.3: JPA/H2 연동
- BE-v0.4: PostgreSQL 프로파일 추가
- BE-v0.5: DTO/검증
- BE-v0.6: JWT 인증 스텁
- BE-v0.7: 간단한 스케줄링/상태 갱신
- BE-v0.8: 전체 안정화 및 대시보드 연동

## Implementation Order (Files)
1. `src/main/java/com/example/minijob/MiniJobApplication.java`
2. `src/main/java/com/example/minijob/HealthController.java`
3. `src/main/java/com/example/minijob/job/Job.java`, `JobRepository.java`, `InMemoryJobRepository.java`
4. `src/main/java/com/example/minijob/job/JobService.java`, `JobController.java`
5. 이후 JPA 구성 파일(`application.yml`, `JobJpaRepository.java`) 순서로 확장

## Troubleshooting
- Gradle wrapper 실행 권한 문제: `chmod +x gradlew`
- 포트 충돌: 8080을 사용 중인 프로세스를 종료하거나 `application.yml`에서 포트 수정
- DB 연결 실패: H2 메모리 모드에서 먼저 실행해 보고, PostgreSQL 연결 정보 확인

## Tutorials
각 단계별 CLI 명령어와 예상 출력은 `TUTORIAL_BE.md`에서 확인할 수 있습니다.
