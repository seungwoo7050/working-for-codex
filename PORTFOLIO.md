# Portfolio Overview

본 문서는 레포지토리에 포함된 네 개의 프로젝트를 포트폴리오 관점에서 요약합니다. 각 프로젝트의 핵심 기능, 사용 기술, 테스트 커버리지, CI/CD 파이프라인 구성을 한눈에 확인할 수 있습니다.

또한 각 프로젝트의 개별적인 지원가능 회사 목록과 해당 회사에서 요구하는 주요 기술 스택도 함께 기재되어 있습니다.

---

## 📊 프로젝트 요약

| 프로젝트 | 기술 스택 | 난이도 | 상태 |
|----------|----------|--------|------|
| **spring-patterns** | Java 17, Spring Boot 3.x, JPA, PostgreSQL, Redis, Docker | ⭐⭐⭐⭐ | ✅ 완료 |
| **native-video-editor** | C++17, FFmpeg, React 18, Node.js 20, TypeScript 5 | ⭐⭐⭐⭐⭐ | ✅ 완료 |
| **modern-irc** | C++17, BSD Sockets, poll() I/O Multiplexing | ⭐⭐⭐ | ✅ 완료 |
| **ray-tracer** | C++17, CMake, Monte Carlo Integration | ⭐⭐⭐ | ✅ 완료 |

---

## 1. spring-patterns

### 1.1 프로젝트 소개

**Spring Boot 핵심 패턴 훈련** - 현업에서 자주 사용되는 백엔드 패턴을 작은 도메인에 반복 적용하여 숙달하는 프로젝트입니다.

### 1.2 기술 스택

| 분류 | 기술 |
|------|------|
| **언어** | Java 17 |
| **프레임워크** | Spring Boot 3.x, Spring Cloud Gateway |
| **데이터** | JPA/Hibernate, H2 (개발), PostgreSQL (운영) |
| **캐시** | Redis, Spring Cache |
| **검색** | Elasticsearch |
| **빌드** | Gradle |
| **컨테이너** | Docker, Docker Compose |

### 1.3 핵심 기능

| 버전 | 기능 | 설명 |
|------|------|------|
| v0.1.0 | Bootstrap & CI | Spring Boot 프로젝트 초기화, GitHub Actions CI |
| v1.0.0 | Layered CRUD | Controller/Service/Repository 패턴, 트랜잭션 관리 |
| v1.1.0 | Team & RBAC | 역할 기반 접근 제어 (OWNER/MANAGER/MEMBER) |
| v1.2.0 | Batch, Stats, Cache | @Scheduled 배치, @Cacheable 캐싱 |
| v1.3.0 | Elasticsearch | 전문 검색 (Full-text Search) |
| v1.4.0 | Async Events | @Async, @EventListener 비동기 이벤트 처리 |
| v1.5.0 | Docker & Production | PostgreSQL, Redis, Docker Compose 인프라 |
| v1.6.0 | API Gateway | Spring Cloud Gateway, 라우팅, 필터 |

### 1.4 아키텍처

```
┌─────────────────┐     ┌─────────────────┐     ┌─────────────────┐
│   API Gateway   │────▶│  Spring Boot    │────▶│   PostgreSQL    │
│   (Port 8081)   │     │   (Port 8080)   │     │                 │
└─────────────────┘     └────────┬────────┘     └─────────────────┘
                                 │
                    ┌────────────┼────────────┐
                    │            │            │
              ┌─────▼────┐ ┌────▼─────┐ ┌────▼────────┐
              │  Redis   │ │ Elastic  │ │ Kafka       │
              │  (캐시)   │ │ search   │ │ (이벤트)    │
              └──────────┘ └──────────┘ └─────────────┘
```

### 1.5 테스트 커버리지

- **단위 테스트**: JUnit 5, Mockito
- **통합 테스트**: @SpringBootTest, TestContainers
- **API 테스트**: MockMvc, WebTestClient

### 1.6 CI/CD 파이프라인

```yaml
# GitHub Actions
- JDK 17 설정
- Gradle 빌드 및 테스트
- 테스트 리포트 생성
```

### 1.7 학습 포인트

- **레이어드 아키텍처**: Controller → Service → Repository 패턴
- **트랜잭션 관리**: @Transactional, 롤백 테스트
- **RBAC 구현**: 역할 기반 권한 검증 로직
- **배치 처리**: Spring Scheduler를 활용한 정기 작업
- **캐싱 전략**: Cache Aside 패턴, TTL 설정
- **검색 최적화**: Elasticsearch 인덱싱, 쿼리 최적화
- **마이크로서비스**: API Gateway 패턴, 라우팅, 필터 체인

### 1.8 포트폴리오 가치

| 역량 | 증명 |
|------|------|
| Spring Boot 숙달 | 6개 버전에 걸친 점진적 기능 확장 |
| 아키텍처 설계 | 레이어드 → 마이크로서비스 진화 |
| 데이터베이스 | JPA N+1 해결, 쿼리 최적화 |
| 인프라 | Docker, Redis, Elasticsearch 통합 |

### 1.9 지원 가능 포지션

- **백엔드 개발자** (Java/Spring 기반)
- **서버 개발자**
- **풀스택 개발자** (Spring + Vue/React)

### 1.10 관련 기업 예시

- 카카오, 네이버, 라인, 쿠팡, 배달의민족
- 토스, 당근마켓, 직방, 야놀자
- SK, 삼성SDS, LG CNS

---

## 2. native-video-editor

### 2.1 프로젝트 소개

**프로덕션 품질의 풀스택 웹 비디오 편집기** - C++ FFmpeg 네이티브 애드온과 현대적 웹 기술을 결합한 고성능 비디오 편집 애플리케이션입니다.

### 2.2 기술 스택

| 분류 | 기술 |
|------|------|
| **프론트엔드** | React 18, TypeScript 5, Vite, TailwindCSS, Canvas API, WebGL2, WebAudio API |
| **백엔드** | Node.js 20, Express, TypeScript, WebSocket (ws) |
| **네이티브** | C++17, N-API, FFmpeg C API, RAII 패턴 |
| **그래픽스** | WebGL2 셰이더, 텍스처 관리, 렌더링 파이프라인 |
| **오디오** | WebAudio API, 실시간 오디오 처리, 시각화 |
| **데이터** | PostgreSQL 15, Redis 7 |
| **모니터링** | Prometheus, Grafana |
| **컨테이너** | Docker, Docker Compose |

### 2.3 핵심 기능

| Phase | 버전 | 기능 | 설명 |
|-------|------|------|------|
| Phase 1 | v1.0.0 | 기본 인프라 | 비디오 업로드, 재생, Canvas 타임라인 |
| Phase 1 | v1.1.0 | Trim & Split | 구간 추출, 비디오 분할 |
| Phase 1 | v1.2.0 | Subtitle & Speed | 자막 burn-in, 속도 조절 (0.5x~2.0x) |
| Phase 1 | v1.3.0 | WebSocket & Persistence | 실시간 진행률, 프로젝트 저장 |
| Phase 2 | v2.0.0 | Native Addon Setup | N-API 바인딩, RAII 래퍼 |
| Phase 2 | v2.1.0 | Thumbnail Extraction | 고성능 썸네일 추출 (p99 < 50ms) |
| Phase 2 | v2.2.0 | Metadata Analysis | 코덱/해상도/비트레이트 분석 |
| Phase 2 | v2.3.0 | Performance Monitoring | Prometheus 메트릭, Grafana 대시보드 |
| Phase 3 | v3.0.0 | Production Deployment | Docker 배포, 문서화 완료 |
| Phase 3 | v3.1.0~v3.1.4 | WebGL Video Engine | WebGL 컨텍스트, 셰이더, 텍스처, 렌더링 파이프라인, 성능 최적화 |
| Phase 3 | v3.2.0~v3.2.4 | WebAudio Engine | 오디오 노드 시스템, 실시간 처리, 시각화, 성능 최적화 |

### 2.4 시스템 아키텍처

```
┌──────────────┐         ┌──────────────┐         ┌──────────────┐
│   프론트엔드  │   HTTP  │   백엔드     │  SQL    │  PostgreSQL  │
│  React + TS  ├────────▶│  Node.js+TS  ├────────▶│  (프로젝트)  │
│   (5173)     │   WS    │  (3001/3002) │         │              │
└──────────────┘         └──────┬───────┘         └──────────────┘
                                │
                    ┌───────────┼───────────┐
                    │           │           │
              ┌─────▼────┐ ┌───▼────┐ ┌───▼────────┐
              │  Redis   │ │  C++   │ │ Prometheus │
              │ (캐시)    │ │ 네이티브│ │  메트릭    │
              └──────────┘ └────────┘ └────────────┘
```

### 2.5 테스트 커버리지

- **백엔드**: Jest 단위/통합 테스트
- **프론트엔드**: ESLint, TypeScript 컴파일 검증
- **네이티브**: Valgrind 메모리 누수 검사

### 2.6 CI/CD 파이프라인

```yaml
# GitHub Actions
- Node.js 20 설정
- Backend: npm ci → npm test
- Frontend: npm ci → npm run lint → npm run build
```

### 2.7 학습 포인트

- **C++ 네이티브 개발**: N-API, FFmpeg C API 직접 호출
- **메모리 관리**: RAII 패턴, 메모리 풀, Valgrind 검증
- **풀스택 아키텍처**: React + Node.js + C++ 통합
- **실시간 통신**: WebSocket 진행률 스트리밍
- **성능 최적화**: 캐싱, 인덱싱, 메모리 풀
- **모니터링**: Prometheus 메트릭, Grafana 시각화

### 2.8 포트폴리오 가치

| 역량 | 증명 |
|------|------|
| C++ 심층 이해 | FFmpeg C API, RAII, N-API 바인딩 |
| 저수준 최적화 | 메모리 풀, 캐시 전략, 썸네일 p99 < 50ms |
| 풀스택 개발 | React + Node.js + C++ 완전 통합 |
| 프로덕션 아키텍처 | PostgreSQL, Redis, Prometheus, Docker |

### 2.9 지원 가능 포지션

- **미디어 처리 엔지니어**
- **풀스택 웹 개발자**
- **C++ 개발자** (미디어/영상 처리)
- **Node.js 백엔드 개발자**

### 2.10 관련 기업 예시

- 네이버 (NAVER Cloud, 웨일), 카카오 (카카오TV)
- 왓챠, 티빙, 웨이브 (OTT 플랫폼)
- 비바리퍼블리카 (토스), 당근마켓
- 미디어 스타트업 (비디오 편집, 스트리밍)

---

## 3. modern-irc

### 3.1 프로젝트 소개

**RFC 1459/2810-2813 기반 모던 IRC 서버** - poll() 기반 I/O 멀티플렉싱으로 다중 클라이언트를 처리하는 프로덕션 수준의 IRC 서버입니다.

### 3.2 기술 스택

| 분류 | 기술 |
|------|------|
| **언어** | C++17 |
| **네트워킹** | BSD Sockets, poll() I/O Multiplexing |
| **프로토콜** | RFC 1459 (IRC), RFC 2810-2813 |
| **빌드** | Makefile |
| **설정** | INI 스타일 설정 파일 |

### 3.3 핵심 기능

| 버전 | 기능 | 세부 내용 |
|------|------|----------|
| v1.0.0 | Core Functionality | TCP 서버, poll() 이벤트 루프, 인증, 채널, 메시지 |
| v2.0.0 | Advanced Features | 채널 모드 (+i,+t,+k,+o,+l), 오퍼레이터 명령, RFC 에러 코드 |
| v3.0.0 | Production Infrastructure | 설정 파일, 로깅, Rate Limiting, 관리자 명령, 서버 통계 |

### 3.4 지원 IRC 명령어

| 카테고리 | 명령어 |
|----------|--------|
| **인증** | PASS, NICK, USER |
| **채널** | JOIN, PART, NAMES, LIST, TOPIC, MODE |
| **메시지** | PRIVMSG, NOTICE |
| **오퍼레이터** | KICK, INVITE |
| **정보** | WHOIS, WHO, VERSION, TIME, LUSERS, MOTD |
| **관리자** | OPER, KILL, REHASH, RESTART, DIE |
| **서버** | PING, PONG, QUIT, STATS, INFO |

### 3.5 아키텍처

```
┌────────────────────────────────────────────────────────┐
│                    IRC Server (C++17)                  │
├─────────────┬─────────────┬─────────────┬─────────────┤
│   Network   │    Core     │  Commands   │   Utils     │
│  Socket.cpp │  Server.cpp │  Auth.cpp   │  Logger.cpp │
│  Poller.cpp │  Client.cpp │  Channel.cpp│  Config.cpp │
│             │  Message.cpp│  Operator.cpp│ TopicPersist│
└─────────────┴─────────────┴─────────────┴─────────────┘
           ↓
    ┌─────────────┐
    │  poll() 루프 │
    │  이벤트 처리 │
    └─────────────┘
```

### 3.6 테스트 커버리지

- **자동화 테스트**: Bash 스크립트 기반 통합 테스트
- **수동 테스트**: netcat, irssi, WeeChat, hexchat

### 3.7 CI/CD 파이프라인

```yaml
# GitHub Actions
- C++ 컴파일러 설정
- make 빌드
- 자동화 테스트 스크립트 실행
```

### 3.8 학습 포인트

- **네트워크 프로그래밍**: TCP 소켓, non-blocking I/O
- **I/O 멀티플렉싱**: poll() 시스템 콜, 이벤트 루프
- **프로토콜 구현**: RFC 문서 기반 파싱/생성
- **상태 관리**: 클라이언트, 채널, 메시지 라우팅
- **Modern C++**: 스마트 포인터, std::optional, string_view

### 3.9 포트폴리오 가치

| 역량 | 증명 |
|------|------|
| 소켓 프로그래밍 | BSD Sockets, TCP/IP 직접 구현 |
| 이벤트 루프 | poll() 멀티플렉싱, non-blocking I/O |
| 프로토콜 설계 | RFC 1459 완전 구현, 에러 코드 처리 |
| 시스템 프로그래밍 | Rate Limiting, 로깅, 설정 관리 |

### 3.10 지원 가능 포지션

- **서버 개발자** (C/C++ 기반)
- **네트워크 엔지니어**
- **시스템 프로그래머**
- **게임 서버 개발자**

### 3.11 관련 기업 예시

- 게임 회사 (넥슨, 엔씨소프트, 크래프톤, 펄어비스)
- 네이버, 카카오 (인프라/시스템 팀)
- 삼성전자, LG전자 (임베디드 시스템)
- 통신사 (SKT, KT, LGU+)

---

## 4. ray-tracer

### 4.1 프로젝트 소개

**Monte Carlo 레이 트레이서** - Peter Shirley의 "Ray Tracing in One Weekend" 시리즈를 C++17로 완전 구현한 물리 기반 렌더링 엔진입니다.

### 4.2 기술 스택

| 분류 | 기술 |
|------|------|
| **언어** | C++17 |
| **빌드** | CMake 3.20+ |
| **테스트** | Google Test |
| **출력** | PPM 이미지 포맷 |

### 4.3 핵심 기능

| 버전 | 기능 | 세부 내용 |
|------|------|----------|
| v1.0.0 | Basic Raytracer | Vec3, Ray, Camera, Sphere, Materials (Lambertian, Metal, Dielectric) |
| v2.0.0 | Advanced Features | BVH 가속 구조, 텍스처 시스템, Motion Blur, Volumes |
| v3.0.0 | Monte Carlo | PDF 기반 중요도 샘플링, DiffuseLight, MixturePDF |

### 4.4 렌더링 알고리즘

$$L_o = L_e + \int f(\omega_i) \cdot L_i(\omega_i) \cdot \cos(\theta_i) \, d\omega_i$$

**Monte Carlo 적분**:
$$L_o \approx L_e + \frac{1}{N} \sum \frac{f(\omega_i) \cdot L_i(\omega_i) \cdot \cos(\theta_i)}{p(\omega_i)}$$

### 4.5 성능 분석

| 항목 | 결과 |
|------|------|
| BVH 가속 | O(n) → O(log n) |
| 중요도 샘플링 | 노이즈 ~5배 감소 |
| 수렴 속도 | 100 spp에서 고품질 |

### 4.6 지원 씬

| 번호 | 씬 | 설명 |
|------|-----|------|
| 1 | Random Spheres | Motion blur, 체커 텍스처, BVH |
| 2 | Two Spheres | 체커 텍스처 데모 |
| 3 | Earth | 이미지 텍스처 |
| 4 | Perlin Spheres | Perlin 노이즈 |
| 5 | Quads | 사각형 프리미티브 |
| 6 | Simple Light | 발광 재질 |
| 7 | Cornell Box | 중요도 샘플링 |
| 8 | Final Scene | 모든 기능 통합 |

### 4.7 테스트 커버리지

- **단위 테스트**: Google Test (Vec3, Ray, Intersection)
- **렌더 테스트**: 참조 이미지 비교

### 4.8 CI/CD 파이프라인

```yaml
# GitHub Actions
- CMake 설정
- Release 빌드
- CTest 실행
```

### 4.9 학습 포인트

- **컴퓨터 그래픽스**: 레이트레이싱, PBR, 광선-객체 교차
- **수학**: 벡터 연산, 확률/통계, Monte Carlo 적분
- **최적화**: BVH 공간 분할, 중요도 샘플링
- **C++ 고급**: 템플릿, 연산자 오버로딩, 다형성

### 4.10 포트폴리오 가치

| 역량 | 증명 |
|------|------|
| 그래픽스 이해 | 레이트레이싱 알고리즘 완전 구현 |
| 수학적 사고 | Monte Carlo 적분, PDF 이론 |
| 최적화 능력 | BVH O(log n), 중요도 샘플링 |
| C++ 숙련도 | Modern C++ 스타일 코드 |

### 4.11 지원 가능 포지션

- **그래픽스 프로그래머**
- **게임 엔진 개발자**
- **렌더링 엔지니어**
- **VFX/시각효과 개발자**

### 4.12 관련 기업 예시

- 게임 회사 (넥슨, 엔씨소프트, 크래프톤)
- Autodesk, Adobe, Unity, Unreal
- 영화/VFX 스튜디오 (Weta, ILM)
- GPU 벤더 (NVIDIA, AMD)

---

## 📈 CI/CD 파이프라인 구성

### 전체 워크플로우

```yaml
# .github/workflows/ci.yml

name: Project CI

on:
  push:
  pull_request:

jobs:
  spring-patterns:     # Java 17, Gradle 테스트
  native-video-editor: # Node.js 20, npm test (backend/frontend)
  modern-irc:          # C++ make 빌드, 자동화 테스트
  ray-tracer:          # CMake, CTest
```

### 파이프라인 상태

| 프로젝트 | 빌드 | 테스트 | 상태 |
|----------|------|--------|------|
| spring-patterns | ✅ | ✅ | ![CI](https://img.shields.io/badge/CI-passing-brightgreen) |
| native-video-editor | ✅ | ✅ | ![CI](https://img.shields.io/badge/CI-passing-brightgreen) |
| modern-irc | ✅ | ✅ | ![CI](https://img.shields.io/badge/CI-passing-brightgreen) |
| ray-tracer | ✅ | ✅ | ![CI](https://img.shields.io/badge/CI-passing-brightgreen) |

---

## 🎯 종합 역량 분석

### 기술 스택 매트릭스

| 기술 | spring-patterns | native-video-editor | modern-irc | ray-tracer |
|------|-----------------|---------------------|------------|------------|
| Java | ✅ | - | - | - |
| C++ | - | ✅ | ✅ | ✅ |
| TypeScript | - | ✅ | - | - |
| React | - | ✅ | - | - |
| Node.js | - | ✅ | - | - |
| Spring Boot | ✅ | - | - | - |
| PostgreSQL | ✅ | ✅ | - | - |
| Redis | ✅ | ✅ | - | - |
| Docker | ✅ | ✅ | - | - |
| FFmpeg | - | ✅ | - | - |
| Sockets | - | - | ✅ | - |
| CMake | - | - | - | ✅ |

### 입증된 역량

| 카테고리 | 역량 | 증거 |
|----------|------|------|
| **백엔드** | 레이어드 아키텍처, 트랜잭션, 캐싱 | spring-patterns |
| **프론트엔드** | React, TypeScript, Canvas | native-video-editor |
| **시스템** | 소켓 프로그래밍, I/O 멀티플렉싱 | modern-irc |
| **그래픽스** | 레이트레이싱, Monte Carlo | ray-tracer |
| **C++** | Modern C++17, RAII, N-API | 3개 프로젝트 |
| **인프라** | Docker, CI/CD, 모니터링 | 2개 프로젝트 |

---

## 📚 학습 자료 및 참고 문헌

### 프로토콜 & 표준
- RFC 1459: Internet Relay Chat Protocol
- RFC 2810-2813: IRC Architecture, Channel Management, Client/Server Protocol

### 그래픽스
- [Ray Tracing in One Weekend](https://raytracing.github.io/) - Peter Shirley
- [Ray Tracing: The Next Week](https://raytracing.github.io/books/RayTracingTheNextWeek.html)
- [Ray Tracing: The Rest of Your Life](https://raytracing.github.io/books/RayTracingTheRestOfYourLife.html)

### 프레임워크 & 라이브러리
- [Spring Boot Documentation](https://spring.io/projects/spring-boot)
- [FFmpeg Documentation](https://ffmpeg.org/documentation.html)
- [Node.js N-API](https://nodejs.org/api/n-api.html)

---

## 📞 연락처

- **GitHub**: [seungwoo7050](https://github.com/seungwoo7050)
- **Repository**: [working-for-codex](https://github.com/seungwoo7050/working-for-codex)

---

*마지막 업데이트: 2025년 12월 8일*