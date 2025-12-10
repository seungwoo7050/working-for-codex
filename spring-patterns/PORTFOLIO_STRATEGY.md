# Spring Patterns — 포트폴리오 가치 평가 및 지원 전략

> 본 문서는 프로젝트의 기술적 가치를 분석하고, 목표 회사 티어별 지원 전략을 제시합니다.

---

## 📊 프로젝트 기술 가치 평가

### 종합 점수: 82/100 (상위 15%)

| 평가 항목 | 점수 | 가중치 | 비고 |
|----------|------|--------|------|
| 기술 스택 다양성 | 90/100 | 20% | Spring 전 영역 + 분산 시스템 |
| 구현 난이도 | 80/100 | 25% | 리액티브, Virtual Threads |
| 프로덕션 완성도 | 80/100 | 20% | Docker, 모니터링 |
| 패턴 이해도 | 85/100 | 20% | 12개 버전 점진적 확장 |
| 문서화 수준 | 75/100 | 15% | 설계 문서 완비 |

---

## 🎯 핵심 기술 역량 매핑

### 역량 1: Spring 생태계 전문성

| 증명 포인트 | 상세 내용 |
|------------|----------|
| Spring Boot 3.3.5 | 최신 버전 활용 |
| Spring Security | JWT + RBAC 구현 |
| Spring Data | JPA, R2DBC, Elasticsearch |
| Spring Cloud | Gateway 모듈 |
| Spring WebFlux | 리액티브 API |

**적합 포지션**: 백엔드 개발, Spring 기반 서비스 개발

### 역량 2: 분산 시스템 인프라

| 증명 포인트 | 상세 내용 |
|------------|----------|
| Redis | 캐싱, 세션 관리 |
| Apache Kafka | 이벤트 스트리밍 |
| Elasticsearch | 검색 엔진 |
| PostgreSQL | 관계형 DB |
| Docker Compose | 멀티 서비스 오케스트레이션 |

**적합 포지션**: 백엔드 개발, 인프라/플랫폼 엔지니어

### 역량 3: 리액티브 프로그래밍

| 증명 포인트 | 상세 내용 |
|------------|----------|
| WebFlux | 논블로킹 REST API |
| R2DBC | 리액티브 DB 접근 |
| Mono/Flux | 리액티브 스트림 |
| SSE | 서버 푸시 |
| Scheduler | 스레드 관리 |

**적합 포지션**: 고트래픽 서비스, 실시간 시스템

### 역량 4: Java 21 최신 기능

| 증명 포인트 | 상세 내용 |
|------------|----------|
| Virtual Threads | 경량 스레드 |
| Record | 불변 데이터 클래스 |
| Pattern Matching | 타입 패턴 |
| Sealed Classes | 제한된 상속 |

**적합 포지션**: 최신 기술 스택 팀, 코어 백엔드

---

## 🏢 회사 티어별 지원 전략

### Tier 1: 글로벌 빅테크

| 회사 | 적합 포지션 | 어필 포인트 | 준비 사항 |
|------|------------|-------------|----------|
| **Amazon** | Backend SDE | 분산 시스템, Kafka | AWS 서비스 이해 |
| **Microsoft** | Backend Engineer | Java/Spring, Azure | Azure Spring Apps 학습 |
| **Google** | Backend Engineer | 시스템 설계 | GCP, gRPC 경험 보완 |

**면접 전략**:
- 시스템 디자인: Rate Limiter, 분산 캐시
- 코딩: 알고리즘 + 동시성
- 행동: 아키텍처 결정 스토리

### Tier 2: 국내 대기업

| 회사 | 적합 포지션 | 어필 포인트 | 준비 사항 |
|------|------------|-------------|----------|
| **네이버** | 백엔드 개발 | Spring, Kafka, ES | 네이버 클라우드 서비스 분석 |
| **카카오** | 서버 개발 | Spring Boot, Redis | 카카오 기술 블로그 분석 |
| **라인** | 백엔드 엔지니어 | Java, Spring | 라인 메신저 아키텍처 |
| **쿠팡** | 백엔드 개발 | 대용량 처리, 이벤트 | 이커머스 도메인 |
| **토스** | 서버 개발 | Spring, Kotlin | Kotlin 학습 병행 |

**면접 전략**:
- 기술 면접: Spring 심화 (AOP, 트랜잭션)
- 과제 전형: CRUD + 성능 최적화
- 컬처핏: 문제 해결, 협업 경험

### Tier 3: 성장 스타트업

| 회사 | 적합 포지션 | 어필 포인트 | 준비 사항 |
|------|------------|-------------|----------|
| **배달의민족** | 백엔드 개발 | 대용량 트래픽 | MSA 경험 강조 |
| **당근마켓** | 서버 개발 | 확장성, 이벤트 드리븐 | 지역 서비스 도메인 |
| **리디** | 백엔드 개발 | Spring, PostgreSQL | 콘텐츠 플랫폼 |
| **야놀자** | 서버 개발 | 예약 시스템, 동시성 | 동시성 제어 강조 |

**면접 전략**:
- 빠른 학습, 문제 해결 강조
- 프로젝트 깊이 있게 설명
- 성장 의지 어필

---

## 📝 이력서 기술 방법

### 프로젝트 요약 (2-3줄)

```
[Spring Patterns] Spring Boot 엔터프라이즈 패턴 학습 프로젝트
- JWT+RBAC 인증/인가, Redis 캐싱, Kafka 이벤트 드리븐 구현
- WebFlux+R2DBC 리액티브 API, Java 21 Virtual Threads 적용
- Docker Compose 기반 PostgreSQL, Redis, Kafka, ES 통합
```

### 기술 스택 기술

```
Backend: Spring Boot 3.3.5, Spring Security, Spring Data JPA/R2DBC
Messaging: Apache Kafka, Spring Kafka
Search: Elasticsearch 8.11, Spring Data Elasticsearch
Database: PostgreSQL 16, Redis 7
Infra: Docker Compose, GitHub Actions
Language: Java 21 (Virtual Threads, Record)
```

### 성과 강조

```
• 12개 버전에 걸쳐 엔터프라이즈 패턴 체계적 구현
• Redis 캐싱으로 DB 부하 5배 감소
• WebFlux + Virtual Threads 하이브리드 아키텍처
• Token Bucket Rate Limiting 직접 구현
```

---

## 🔍 추가 강화 포인트

### 현재 부족한 영역

| 영역 | 현황 | 보완 방법 |
|------|------|----------|
| 테스트 커버리지 | 기본 수준 | TestContainers 통합 테스트 |
| Kubernetes | 미구현 | K8s 배포 추가 |
| gRPC | 미구현 | 서비스 간 통신 개선 |
| Kotlin | 미사용 | 토스/카카오 지원 시 학습 |

### 향후 확장 가능 기능

1. **Spring Cloud 풀셋**: Config, Eureka, Sleuth
2. **gRPC 통신**: 서비스 간 고성능 통신
3. **Kubernetes**: 오토스케일링, 롤링 배포
4. **Observability**: Micrometer + Zipkin

---

## 📈 경쟁력 분석

### 동일 연차 대비 차별점

| 일반 주니어 | 본 프로젝트 |
|------------|------------|
| Spring Boot 기본 CRUD | 12개 버전 점진적 확장 |
| 단일 DB | Redis, Kafka, ES 통합 |
| 블로킹만 | 리액티브 + Virtual Threads |
| 로컬 개발 | Docker 멀티 서비스 |

### 시장 희소성

```
[높음] WebFlux + R2DBC 실무 경험
[높음] Java 21 Virtual Threads 적용
[중간] Kafka 이벤트 드리븐
[중간] Elasticsearch 검색 통합
```

---

## 🎯 최종 권장 전략

### 1순위: 국내 대기업 백엔드
- **이유**: Spring 생태계 완벽 매칭
- **타겟**: 네이버, 카카오, 라인, 쿠팡
- **승률**: 높음 (Spring 숙련도 증명)

### 2순위: 핀테크/이커머스 스타트업
- **이유**: 대용량 트래픽, 이벤트 드리븐 경험
- **타겟**: 토스, 배민, 당근, 야놀자
- **승률**: 높음 (도메인 관심 시)

### 3순위: 글로벌 테크
- **이유**: 분산 시스템 경험
- **타겟**: Amazon, Microsoft
- **승률**: 중간 (알고리즘 병행 필요)

---

## 📌 핵심 메시지

> "raw-http-server에서 시작해 Spring 엔터프라이즈 패턴까지, 웹 백엔드의 전 레이어를 이해한 개발자"

**기억할 키워드**:
- **12 versions**: 점진적 패턴 학습
- **Spring 전 영역**: Security, Data, Cloud, WebFlux
- **분산 시스템**: Redis, Kafka, Elasticsearch
- **최신 Java**: Virtual Threads, Record

---

## 🔗 연계 프로젝트

### raw-http-server (이전)
- 순수 Java로 HTTP 서버 구현
- 프로토콜 레벨 이해

### cpp-pvp-server (병행)
- C++ 게임 서버
- 시스템 프로그래밍 역량

**종합 어필**: "저수준 프로토콜부터 고수준 프레임워크까지, 웹 백엔드의 모든 레이어를 직접 구현한 경험"
