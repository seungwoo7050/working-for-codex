# Portfolio Overview

본 문서는 레포지토리에 포함된 네 개의 프로젝트를 포트폴리오 관점에서 요약합니다. 각 프로젝트의 핵심 기능, 사용 기술, 테스트 커버리지, CI/CD 파이프라인 구성을 한눈에 확인할 수 있습니다.

## spring-patterns
- **주요 기능**: Spring Boot 기반 레이어드 CRUD, RBAC, 배치/통계/캐시, Elasticsearch 검색, Kafka 비동기 이벤트, Docker·PostgreSQL·Redis 인프라 등 실무 패턴을 단계별로 통합.
- **사용 기술**: Java 17, Spring Boot 3.x, JPA, PostgreSQL, Redis, Kafka, Elasticsearch, Gradle.
- **테스트 커버리지**: JUnit 5와 Spring Boot Test를 기반으로 각 마일스톤의 기능 검증을 목표로 하며, CI에서 `./gradlew test`를 통해 전 범위를 실행합니다.
- **CI/CD**: GitHub Actions에서 JDK 17 설정 후 Gradle 캐싱을 활용해 테스트 파이프라인을 수행합니다.

## native-video-editor
- **주요 기능**: React 18·TypeScript 5 프론트엔드와 Node.js 20 백엔드, FFmpeg C API 기반 C++ 네이티브 애드온을 결합한 프로덕션급 웹 동영상 편집기. PostgreSQL·Redis·Prometheus·Docker를 통한 운영 준비.
- **사용 기술**: React, Vite, TailwindCSS, TypeScript, Node.js/Express, FFmpeg C API, C++17, WebSocket.
- **테스트 커버리지**: 프론트엔드 린트·빌드, 백엔드 단위 테스트와 N-API 래퍼 로직 검증을 npm 스크립트로 제공하며, CI에서 백엔드 `npm test`와 프론트엔드 `npm run lint && npm run build`를 실행합니다.
- **CI/CD**: GitHub Actions가 Node.js 20 환경을 설정하고 npm 캐시를 활용하여 린트·빌드·테스트를 병렬로 수행합니다.

## modern-irc
- **주요 기능**: RFC 1459/2810-2813을 준수하는 C++17 IRC 서버. poll 기반 I/O 멀티플렉싱, 채널/사용자 상태 관리, 운영자 명령과 모드, 로깅·레이트리미팅·설정 파일 지원.
- **사용 기술**: C++17, BSD 소켓, poll(), Makefile 기반 빌드.
- **테스트 커버리지**: 통합 스크립트(`tests/integration/test_phase2.sh`)로 주요 명령 흐름을 검증하며, 향후 추가 명령별 단위 테스트 확장을 고려합니다.
- **CI/CD**: GitHub Actions에서 리포지토리를 체크아웃한 뒤 `make`로 서버를 빌드하여 기본 컴파일 품질을 확인합니다.

## ray-tracer
- **주요 기능**: "Ray Tracing in One Weekend" 시리즈 전체를 C++17로 구현. BVH 가속구조, 다양한 텍스처·재질, 중요도 샘플링 기반 Monte Carlo 렌더링과 PDF 구성 요소, ONB 좌표 변환 유틸리티를 포함합니다.
- **사용 기술**: C++17, CMake, Google Test, Monte Carlo 렌더링 기법.
- **테스트 커버리지**: Google Test 기반 단위 테스트로 벡터 수학, 광선, 재질, BVH, 텍스처, PDF, ONB 등을 검증합니다.
- **CI/CD**: GitHub Actions에서 CMake 구성 후 빌드 및 `ctest` 실행으로 단위 테스트를 자동화합니다.
