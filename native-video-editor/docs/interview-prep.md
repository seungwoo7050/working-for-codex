# Interview Approach & Technical Question Prep (VoyagerX)

This document tailors interview prep to the VoyagerX Vrew web application role, with emphasis on native-video-editor as the anchor project.

## Position Fit Narrative
- **Why this project**: Full-stack + native video processing aligns with Vrew’s stack (React, Node.js, TypeScript, FFmpeg, WebGL/WebAudio optional). Highlight Phase 3 completeness and FFmpeg/N-API depth.
- **Impact story**: Real-time collaboration, low-latency FFmpeg calls, and observability/monitoring show production readiness even under constrained resources.
- **Differentiators**: Hands-on C++17 + FFmpeg integration, WebSocket collaboration model without heavy message brokers, and Prometheus/Grafana monitoring.

## Interview Approach
- **Structure answers** with STAR (Situation, Task, Action, Result) plus metrics (latency, error rates, throughput) from your project logs or benchmarks.
- **Emphasize trade-offs**: N-API vs. pure JS/wasm, CPU vs. GPU acceleration, Redis cache eviction policies, schema evolution via migrations.
- **Map to job requirements**: Show how native-video-editor covers React/Node/TypeScript/FFmpeg; mention exploratory plans for WebGL/WebAudio acceleration where applicable.
- **Pairing & collaboration**: Describe code review habits, observability dashboards, and runbook drills to show reliability mindset.

## Technical Question Cheat Sheet
### General (app developer / CS fundamentals)
- **PNG vs JPG**: PNG = lossless, alpha channel, DEFLATE compression; JPG = lossy DCT-based, better for photos; note file size vs. fidelity trade-off.
- **Dynamic Programming**: Optimal substructure + overlapping subproblems; memoization/tabulation examples (edit distance, knapsack).
- **Virtual Memory**: Abstraction layer mapping virtual → physical addresses with paging; benefits: isolation, security, overcommit; discuss TLB and page faults.
- **Garbage Collection**: Automated memory management (mark-sweep/generational); pauses vs. throughput trade-offs and why Node.js V8 GC tuning can matter for latency.
- **Cache**: Hierarchical caches (CPU L1/L2/L3, Redis app cache); hit ratio, eviction (LRU/LFU), and consistency considerations.
- **DB Index Pros/Cons**: Faster reads via secondary indexes; costs: write amplification, storage, and potential suboptimal planner choices if over-indexed.
- **Asymmetric Crypto**: Public/private key pairs for encryption/signing; use in TLS handshakes and JWT signing; slower than symmetric, often combined (hybrid crypto).
- **Storage Performance**: HDD < SSD < DRAM in latency/throughput; mention NVMe and access patterns (sequential vs random).
- **Git Advantages**: Distributed VCS, cheap branching, offline commits, strong integrity via hashes; enables PR-based workflows.

### Vrew Web App-Specific (pick at least 5 to answer deeply)
- **UTF-8**: Variable-length encoding (1–4 bytes) for Unicode; backward-compatible with ASCII; important for subtitles/metadata.
- **React**: Component-based UI; hooks for state/effects; reconciliation and virtual DOM; performance via memoization/splitting.
- **Reflow**: Layout recalculation in browsers; minimize via batching state changes, avoiding layout thrash, and using CSS transforms vs. layout properties.
- **Functional Programming**: Immutability, pure functions, higher-order functions; helps predictability in React state management.
- **Callback Hell**: Pyramid of nested callbacks; mitigated by Promises/async-await and structured concurrency.
- **Async I/O**: Non-blocking operations in Node.js event loop; tie to FFmpeg job orchestration and WebSocket handling.
- **IPC**: Mechanisms for inter-process communication (pipes, sockets, shared memory); relate to native-addon boundaries and worker processes.
- **C++ Polymorphism**: Compile-time (templates/overloading) and runtime (virtual dispatch); relevant for extensible FFmpeg wrapper design.
- **GPU Acceleration**: Using GPU via FFmpeg hwaccel/WebGL for filters/encoding; discuss data transfer costs and fallbacks.

## Demo & Code Walkthrough Plan
1. **Architecture first**: Show the diagram, explain data flows, and how observability supports ops.
2. **Deep dive**: Native addon interface (error handling, memory safety), WebSocket collaboration, and key migrations.
3. **Reliability**: Mention runbook drills, rollback plan, and alert thresholds.
4. **Performance**: Share any p95/p99 latency numbers and tuning steps (caching, batching, hardware acceleration flags).
5. **Future work**: WebGL/WebAudio experiments for client-side acceleration; CI/CD integration with cloud services.

## On-the-spot Coding/Design Tips
- Clarify requirements, list constraints, propose a baseline design, then optimize.
- Communicate trade-offs: correctness, latency, complexity, and operability.
- Keep examples close to the codebase (FFmpeg job routing, WebSocket broadcast patterns, schema migrations).

## Code-backed answers for the Vrew technical questions
Use these as quick, implementation-grounded talking points (pick 5+).

- **UTF-8이란?** N-API 경계에서 `Utf8Value()`로 경로/코덱 문자열을 UTF-8로 안전하게 풀어 FFmpeg에 전달한다는 예시로 답변합니다. 이때 예외는 JS로 전파해 입력 검증과 연계합니다. 【F:native/src/video_processor.cpp†L43-L69】【F:native/src/video_processor.cpp†L139-L187】
- **React란?** 컴포넌트/훅 기반 설계로 업로드→재생→타임라인→편집 패널을 상태로 묶어 UI를 선언형으로 구성한 `App.tsx`를 근거로 설명합니다. WebSocket 진행률 표시까지 상태로 결합된 사례를 언급합니다. 【F:frontend/src/App.tsx†L1-L118】【F:frontend/src/App.tsx†L120-L169】
- **Reflow란?** DOM 레이아웃 재계산을 줄이려 타임라인을 Canvas 한 장에 그리는 방식을 예로 듭니다. `Timeline.tsx`가 DPR 보정 후 캔버스에 눈금/플레이헤드를 직접 그려, 다수의 DOM 노드 없이 리플로우를 최소화합니다. 【F:frontend/src/components/Timeline.tsx†L1-L95】
- **함수형 프로그래밍 패러다임이란?** React 훅에서 불변 데이터와 순수 함수로 상태를 교체하는 패턴을 보여 줍니다. 예를 들어 `handleTrimComplete`가 기존 상태를 복사해 새 편집 결과를 설정하고, 파생 `VideoMetadata`를 계산하는 순수 변환으로 구성됩니다. 【F:frontend/src/App.tsx†L26-L55】
- **Callback hell이란? / async I/O란?** Express 라우터에서 FFmpeg 작업을 `async/await`로 직렬화하여 중첩 콜백 없이 비동기 I/O(파일 존재 확인→FFmpeg 변환→응답)를 처리한 예로 설명합니다. 【F:backend/src/routes/edit.routes.ts†L10-L115】
- **프로세스 간 통신(IPC)이란?** Node.js와 C++ 네이티브 애드온 사이를 N-API 객체 래핑으로 연결한 `ThumbnailExtractorWrapper`/`MetadataAnalyzerWrapper`가 JS↔FFmpeg 사이의 안전한 IPC 계층 역할을 한다고 답변합니다. 【F:native/src/video_processor.cpp†L8-L110】【F:native/src/video_processor.cpp†L139-L195】
- **C++에서 다형성이란?** FFmpeg 래퍼는 표준 스마트 포인터와 `ObjectWrap`을 활용해 인터페이스를 캡슐화합니다. 필요 시 `MetadataAnalyzer`처럼 동일 인터페이스(메타데이터 추출/코덱 지원 여부)로 확장 가능한 구조를 예로 들며, N-API 객체 래핑이 런타임에 다른 구현체로 대체 가능한 형태임을 설명합니다. 【F:native/src/video_processor.cpp†L8-L110】【F:native/src/metadata_analyzer.cpp†L12-L106】
- **그래픽카드를 이용한 하드웨어 가속이란?** 현재 타임라인은 Canvas 기반이지만, `PROJECT-COMPLETION.md`에 WebGL 등 GPU 가속 선호와 캔버스 대안을 명시해 두었습니다. 이를 근거로 GPU hwaccel(ffmpeg `hwaccel` 플래그)이나 WebGL로 확장할 계획과 데이터 이동 비용 고려점을 설명합니다. 【F:docs/PROJECT-COMPLETION.md†L180-L223】
