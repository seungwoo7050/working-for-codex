# 스냅샷 001: Bootstrap (v0.1.0)

## 개요
- **대응 버전**: v0.1.0
- **패치 범위**: 0.1.1 ~ 0.1.6
- **빌드 가능**: ✅ 예
- **테스트 가능**: ✅ 예

## 포함된 기능
- Spring Boot 3.3.5 프로젝트 초기화
- HealthController (`GET /api/health`)
- GlobalExceptionHandler (기본 예외 처리)
- 기본 테스트 (컨텍스트 로딩, 헬스 체크)

## 빌드 방법
```bash
cd snapshots/001_bootstrap
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

## 검증
```bash
curl http://localhost:8080/api/health
# 예상 응답: {"status":"OK","timestamp":"2025-..."}
```
