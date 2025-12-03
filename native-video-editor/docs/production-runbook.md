# Production Runbook

This runbook documents how to operate the native-video-editor stack in production-like environments, including deployment, monitoring, troubleshooting, and recovery procedures.

## 1) System Overview
- **Frontend**: React + TypeScript SPA built with Vite. Delivered via CDN or static hosting behind a TLS-enabled reverse proxy.
- **Backend**: Node.js (Express + WebSocket) API server that orchestrates edits, media jobs, and collaboration state.
- **Native Addon**: N-API C++17 module that wraps FFmpeg for low-latency media transforms.
- **Data Stores**: PostgreSQL (primary transactional DB) and Redis (cache + ephemeral collaboration state).
- **Observability**: Prometheus (metrics), Grafana (dashboards), structured JSON logs, and alerting via Alertmanager (or provider of choice).
- **Packaging**: Docker images per service; docker-compose or Kubernetes for orchestration.

## 2) Environments & Configuration
- **Profiles**: `dev`, `staging`, `prod`. Use environment-variable overrides per profile.
- **Secrets**: Store JWT secrets, DB credentials, and FFmpeg hardware acceleration flags in a secrets manager (e.g., AWS SSM/Secrets Manager). Never bake into images.
- **Feature Flags**: Use environment toggles for hardware acceleration, thumbnail generation, and experimental codecs.
- **Config Validation**: On boot, validate required env vars (DB URL, Redis URL, JWT secret, FFmpeg path, Prometheus scrape config). Fail fast if missing.

## 3) Deployments
- **Images**: Build versioned Docker images for `frontend`, `backend`, `native-addon` (bundled with backend where applicable).
- **Migrations**: Run `migrations/` against PostgreSQL before rolling out the new backend image. Use `npm run migrate` (or project-specific script) inside a migration job.
- **Order of operations**:
  1. Drain traffic (load balancer connection draining).
  2. Run DB migrations.
  3. Deploy backend (include native addon build step if separate).
  4. Deploy frontend static assets (cache-busted build) to CDN/storage.
  5. Re-enable traffic and warm caches for critical routes (e.g., project list, recent edits).
- **Rollbacks**:
  - Keep previous image tags and migration rollback scripts available. If migration is irreversible, use blue/green and cut traffic back to the old stack.
  - For frontend, redeploy prior artifact version; ensure cache invalidation on CDN.

## 4) Monitoring & Alerting
- **Health**: `/healthz` for backend (checks DB/Redis/native module presence). Frontend served asset availability via synthetic checks.
- **Metrics**: Export Prometheus metrics for request latency (p50/p95/p99), error rates, FFmpeg job durations, queue depth, memory/CPU usage, WebSocket session counts.
- **Dashboards**: Grafana boards for API SLOs, FFmpeg throughput, database health (connections, replication lag if any), and Redis hit ratio.
- **Logging**: Structured JSON logs with request IDs and correlation IDs propagated to FFmpeg jobs. Ship to centralized log storage (e.g., Loki/ELK).
- **Alerts (suggested)**:
  - HTTP 5xx or WebSocket error rate exceeds threshold (e.g., >2% for 5m).
  - FFmpeg job p99 latency or failure ratio exceeds baseline.
  - PostgreSQL connection pool exhaustion, slow queries, or replication lag (if configured).
  - Redis hit ratio drop or memory eviction spikes.
  - Host/container CPU, memory, and disk usage nearing limits.

## 5) Troubleshooting
- **Recent regressions**: Compare metrics before/after deploy; if elevated errors align with deploy, initiate rollback.
- **FFmpeg failures**: Inspect backend logs for native-addon error codes; run a canary FFmpeg command in the same container to verify codec availability and GPU flags.
- **WebSocket issues**: Check Redis/pub-sub connectivity and load balancer idle timeouts. Validate CORS/origin settings.
- **Performance degradation**: Inspect DB slow query logs; confirm indices for high-traffic queries. Verify CDN cache headers for static assets. Review heap/CPU profiles for Node process.
- **Data integrity**: If migration failed mid-way, lock writes, restore from latest snapshot, and re-run migrations after fix.

## 6) Backup & Recovery
- **PostgreSQL**: Daily snapshots + WAL archiving; test point-in-time recovery quarterly. Keep runbooks for promote/failover if using replicas.
- **Redis**: AOF or snapshotting for critical state; otherwise treat as ephemeral and rebuild from DB.
- **Artifacts**: Store build artifacts and previous image tags for fast rollback.
- **Disaster recovery**: Document RPO/RTO targets. Practice restore-from-backup drills and load balancer failover.

## 7) Security & Compliance
- **Transport security**: Enforce HTTPS/TLS end-to-end; HSTS on frontend domains.
- **Access control**: Least-privilege IAM for CI/CD, DB, and object storage. Rotate credentials periodically.
- **Dependency hygiene**: Automated dependency scanning and OS image updates.
- **Content safety**: Validate and sanitize user-uploaded media filenames/paths; enforce size and duration limits.

## 8) Runbook Exercises
- Quarterly game days for: DB failover, Redis outage, FFmpeg codec regression, and WebSocket broadcast failures.
- After each incident, capture a postmortem with timeline, contributing factors, and action items.
