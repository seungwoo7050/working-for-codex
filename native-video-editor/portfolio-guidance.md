# Portfolio Guidance for Native Video Editor

## Extending Cloud, CI/CD, and Logging Coverage
- **Why extend beyond Phase 3**: Phase 3 focuses on Docker Compose and Prometheus/Grafana for observability, but hiring teams often expect evidence of cloud deployment skills (AWS/GCP/Azure), managed database/cache usage, and CI/CD pipelines that target those environments.
- **Suggested additions**:
  - **Cloud baseline**: Deploy the backend to a managed container service (e.g., AWS ECS/Fargate or GCP Cloud Run) and connect to managed PostgreSQL/Redis. Document infrastructure-as-code snippets (Terraform/CDK) and secrets management (AWS SSM/Secrets Manager).
  - **CI/CD**: Add a pipeline that runs lint/test/build, publishes Docker images to a registry, and performs staged deploys (dev/staging/prod) with rollout gates (health checks, smoke tests). GitHub Actions examples: matrix testing for Node/C++ layers; separate jobs for Docker build/push; deploy job using OIDC to assume cloud roles.
  - **Logging/monitoring**: Include structured JSON logging (request IDs, user IDs, correlation IDs), log shipping to a centralized stack (e.g., OpenTelemetry → OTLP collector → CloudWatch/Stackdriver/Elastic), and alerting rules tied to SLOs (p99 latency, error rates). Show example log queries and dashboards.
  - **Security hardening**: Add dependency scanning (npm/cargo/audit), container image scanning, and secret scanning in CI; enable TLS termination and WAF/CDN configuration for the frontend assets and API gateway.
- **Expected outcome**: A short “production runbook” that proves you can take the Phase 3 system and operate it in a cloud environment with automated delivery, observability, and security controls.

## Parallel Practice Projects (React and NestJS)
- **React Web Patterns**: Completing the milestones (CRUD, auth/RBAC, stats, search, tests, accessibility) complements the video editor by demonstrating modern SPA patterns, strong typing, and testing depth. It is suitable to run in parallel if you time-box features and reuse API mock tools (MSW/json-server) to avoid backend bottlenecks.
- **NestJS Patterns**: The milestone set (layered architecture, RBAC, batch/cache, Elasticsearch, Kafka, production infra) gives backend credibility and pairs well with the native-video-editor backend. Prioritize if you want to highlight Node backend or distributed-system awareness.
- **Workload guidance**: If time is limited, finish the React project first for frontend polish and interview demos, then the NestJS project for backend breadth. Running both in parallel is feasible if you share domain models (issue tracker, products) and reuse Docker Compose services to reduce setup overhead.

## VoyagerX Web App Developer Application Strategy
- **Lead with the video editor**: Present the native-video-editor as the centerpiece since it matches the Vrew stack (React, Node.js, TypeScript, FFmpeg, and comfort going “lower” with C++/WebGL/WebAudio aspirations). Emphasize Phase 3 delivery, C++17 N-API bindings to FFmpeg, real-time WebSocket collaboration, and monitoring/alerting—these map directly to the role’s “웹 기반 동영상 편집기” scope.
- **Selective supporting projects**:
  - **React patterns project (recommended)**: Use it to showcase UI/UX polish, routing/auth patterns, and testing maturity. This helps answer questions about frontend craft (1px alignment, smooth UX) and modern React practices beyond the editor codebase.
  - **NestJS patterns project (optional but additive)**: Good for highlighting Node backend depth (RBAC, cache, batch jobs, search, messaging) and cloud-ready Docker-compose skills. Include only if time permits; it aligns with the role’s Node.js requirement but is secondary to the editor.
  - **Ray tracer (optional)**: Keep as a “depth in C++/math/performance” appendix. Mention briefly to show low-level aptitude, but avoid diluting the main narrative unless the interviewer shows graphics interest.
  - **Raw HTTP server in Python (low priority)**: Not required for this web app role. Reference it only if asked about network fundamentals; otherwise, focus prep time on editor + React/NestJS patterns.
- **Interview angle and tech-question prep**:
  - Map your experiences to the listed questions: e.g., structured logging + cache strategy for “Cache란?”, memory safety and valgrind runs for “Garbage Collection/Virtual Memory란?”, codec handling for “UTF-8/PNG vs JPG,” async pipelines for “async I/O,” and C++ polymorphism for the N-API layer.
  - Prepare concrete anecdotes: performance wins (p99 latency, FFmpeg throughput), debugging stories (memory leaks, WebSocket race conditions), and UX polish examples (frame-accurate scrubbing, keyboard shortcuts) that show “더 빨리 더 잘” 개선 노력.
  - Bring a short “production runbook” and architecture diagram to demonstrate operational readiness (deploy, observe, rollback) even in a tough market.
