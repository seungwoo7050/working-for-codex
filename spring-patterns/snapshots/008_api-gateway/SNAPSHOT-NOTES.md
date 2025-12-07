````markdown
# 스냅샷 008: API Gateway (v1.6.0)

## 개요
- **대응 버전**: v1.6.0
- **패치 범위**: 1.6.1 ~ 1.6.5
- **빌드 가능**: ⚠️ Docker 환경 권장
- **테스트 가능**: ⚠️ Backend 서비스 필요

## 포함된 기능
- v1.5.0의 모든 기능 (Production Infrastructure)
- Spring Cloud Gateway 모듈
- 라우팅 규칙 (/api/** → Backend)
- 로깅 필터 (요청/응답 로깅)
- CORS 설정 (Gateway에서 중앙 처리)
- Docker Compose 통합

## 새로 추가된 컴포넌트

### Gateway 모듈 구조
```
gateway/
├── build.gradle
├── settings.gradle
├── Dockerfile
├── gradlew
└── src/
    └── main/
        ├── java/
        │   └── com/example/gateway/
        │       ├── GatewayApplication.java
        │       ├── config/
        │       │   └── CorsConfig.java
        │       └── filter/
        │           └── LoggingFilter.java
        └── resources/
            └── application.yml
```

### 주요 클래스
- `GatewayApplication`: Spring Cloud Gateway 진입점
- `LoggingFilter`: 전역 요청/응답 로깅
- `CorsConfig`: CORS 설정

### 라우팅 규칙
```yaml
routes:
  - id: backend-api
    uri: http://${BACKEND_HOST}:${BACKEND_PORT}
    predicates:
      - Path=/api/**
```

## 포트 매핑
| 서비스 | 포트 | 설명 |
|--------|------|------|
| Gateway | 8081 | 외부 노출 (클라이언트 접근) |
| Backend | 8080 | 내부 (Gateway 통해서만 접근) |

## 빌드 방법
```bash
# Backend 빌드
cd snapshots/008_api-gateway
./gradlew build

# Gateway 빌드
cd gateway
./gradlew build
```

## Docker 실행 방법
```bash
# 전체 스택 실행
docker-compose up -d

# Gateway 통해 접근
curl http://localhost:8081/api/health

# 로그 확인
docker logs spring-gateway -f
```

## 테스트 방법
```bash
# Gateway 라우팅 테스트
curl http://localhost:8081/api/health

# CORS 테스트
curl -H "Origin: http://localhost:3000" \
     -H "Access-Control-Request-Method: GET" \
     -X OPTIONS \
     http://localhost:8081/api/health -v
```

## 마이그레이션 노트
- 클라이언트 API URL: `http://localhost:8080` → `http://localhost:8081`
- Backend의 `@CrossOrigin` 어노테이션 제거 가능 (Gateway에서 처리)
- JWT 인증은 Backend에서 유지 (선택적으로 Gateway로 이동 가능)
````
