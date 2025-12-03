# Architecture Overview

This document describes the high-level architecture of the native-video-editor system and shows data/control flows between components.

## Diagram
```mermaid
graph LR
    subgraph Client
        UI[React + TypeScript SPA]
    end

    subgraph Backend[Backend (Node.js)]
        API[REST & WebSocket API]
        Queue[Job Orchestrator]
        Native[N-API C++17 FFmpeg Wrapper]
    end

    subgraph Data[Data Stores]
        PG[(PostgreSQL)]
        Redis[(Redis Cache / PubSub)]
    end

    subgraph Observability[Observability]
        Prom[Prometheus]
        Graf[Grafana]
        Logs[Structured Logs]
    end

    CDN[CDN/Static Hosting]

    UI -->|HTTPS| CDN
    UI -->|API + WS| API
    API --> Queue
    Queue --> Native
    API <--> PG
    API <--> Redis
    Queue <--> Redis
    Prom -.-> API
    Prom -.-> Queue
    Prom -.-> Native
    Logs -.-> Observability
    Graf --> Prom
```

## Component Responsibilities
- **Frontend (React SPA)**: Provides the editing UI, synchronizes state via REST/WS, streams media previews, and uses cache-busted static assets served from CDN.
- **Backend (Node.js)**: Hosts REST endpoints and WebSocket channels for collaborative editing, routes editing commands to the native addon, and handles authentication/authorization.
- **Native Addon (N-API + FFmpeg)**: Executes CPU/GPU-accelerated video operations (trim, transcode, effects) with minimal latency; exposes a safe API to Node.js.
- **PostgreSQL**: System of record for projects, timelines, assets, and user/session metadata. Supports migrations for schema evolution.
- **Redis**: Low-latency cache and pub/sub channel for WebSocket fan-out and ephemeral collaboration state.
- **Observability Stack**: Prometheus scrapes metrics from backend/native processes; Grafana visualizes SLOs; structured JSON logs feed a centralized log sink.
- **Deployment**: Containerized services (frontend, backend + native addon) deployed via docker-compose or Kubernetes, fronted by TLS-terminated reverse proxy and CDN for assets.

## Data & Control Flows
1. The user loads the SPA from CDN; subsequent API/WS traffic hits the backend.
2. Editing commands and timeline changes are sent over WebSocket/REST; the backend validates auth and routes media mutations to the native addon.
3. FFmpeg-powered addon processes media and emits progress/results back to the backend, which updates Redis and notifies subscribed clients.
4. Persistent state (projects, edits, user data) is stored in PostgreSQL; Redis caches hot reads and coordinates collaboration events.
5. Metrics and logs are emitted continuously for alerting and debugging; deployments roll out via container images with schema migrations.
