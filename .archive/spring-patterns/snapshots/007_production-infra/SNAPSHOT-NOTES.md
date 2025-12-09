````markdown
# 스냅샷 007: Production Infrastructure (v1.5.0)

## 개요
- **대응 버전**: v1.5.0
- **패치 범위**: 1.5.1 ~ 1.5.5
- **빌드 가능**: ⚠️ Docker 환경 필요 (PostgreSQL, Redis)
- **테스트 가능**: ⚠️ Docker 환경 필요

## 포함된 기능
- v1.4.0의 모든 기능 (Async Events, Kafka)
- PostgreSQL 16 데이터베이스 지원
- Redis 7 분산 캐시
- Docker & docker-compose 기반 전체 스택
- 멀티스테이지 Dockerfile

## 새로 추가된 컴포넌트

### Configuration
- application-prod.yml (PostgreSQL, Redis 설정)

### Infrastructure
- Dockerfile (멀티스테이지 빌드)
- docker-compose.yml (전체 스택)

### 환경 프로파일
- **local**: H2 인메모리, Simple 캐시
- **test**: H2 인메모리, Simple 캐시
- **prod**: PostgreSQL, Redis 캐시

## 주요 변경사항
- H2 → PostgreSQL 전환 (prod 프로파일)
- Simple 캐시 → Redis 전환 (prod 프로파일)
- 컨테이너 오케스트레이션 추가

## 빌드 방법
```bash
cd snapshots/007_production-infra
./gradlew build
```

## Docker 실행 방법
```bash
# 전체 스택 실행
docker-compose up -d

# 애플리케이션만 빌드 및 실행
docker-compose up --build app

# 상태 확인
docker-compose ps
```

## 테스트 방법
```bash
# 단위 테스트 (H2 사용)
./gradlew test

# 통합 테스트 (Docker 필요)
docker-compose up -d db redis
./gradlew integrationTest
```

## 환경 변수
| 변수 | 기본값 | 설명 |
|------|--------|------|
| DB_HOST | localhost | PostgreSQL 호스트 |
| DB_PORT | 5432 | PostgreSQL 포트 |
| DB_NAME | training | 데이터베이스 이름 |
| DB_USER | app | 데이터베이스 사용자 |
| DB_PASSWORD | app | 데이터베이스 비밀번호 |
| REDIS_HOST | localhost | Redis 호스트 |
| REDIS_PORT | 6379 | Redis 포트 |
````
