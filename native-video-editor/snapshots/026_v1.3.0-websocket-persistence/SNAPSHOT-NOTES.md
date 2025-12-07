# 026_v1.3.0-websocket-persistence 스냅샷

## 📋 패치 정보

| 항목 | 값 |
|------|-----|
| 패치 범위 | 1.3.1 ~ 1.3.7 |
| 버전 태그 | v1.3.0 |
| 이름 | WebSocket & Persistence |
| 이전 단계 | v1.2.0 (Subtitle & Speed) |

## 🎯 이 스냅샷의 목표

실시간 진행률 표시 및 프로젝트 영속 저장 기능을 구현합니다.

### 추가된 기능
- **WebSocketService**: 실시간 양방향 통신 (ws 라이브러리)
- **useWebSocket 훅**: 클라이언트 WebSocket 연결 관리
- **PostgreSQL 마이그레이션**: projects 테이블 생성
- **DatabaseService**: PostgreSQL 연결 풀 관리
- **Redis 캐싱**: 메타데이터 캐싱
- **project.routes.ts**: 프로젝트 CRUD API
- **ProjectPanel 컴포넌트**: 프로젝트 저장/불러오기 UI
- **ProgressBar 컴포넌트**: 실시간 진행률 표시

## 📁 포함되어야 할 파일

### v1.2.0에서 상속
- 모든 v1.2.0 파일들

### 신규 추가 파일
```
frontend/src/
├── components/
│   ├── ProjectPanel.tsx      # 프로젝트 관리 UI
│   └── ProgressBar.tsx       # 진행률 표시
├── hooks/
│   ├── useWebSocket.ts       # WebSocket 연결
│   └── useProjects.ts        # 프로젝트 CRUD

backend/src/
├── ws/
│   └── websocket.service.ts  # WebSocket 서버
├── db/
│   ├── database.service.ts   # PostgreSQL
│   └── redis.service.ts      # Redis
├── routes/
│   └── project.routes.ts     # 프로젝트 API

migrations/
└── 001_initial_schema.sql    # DB 스키마
```

## 🔧 추가 요구사항

- **PostgreSQL 15** 실행 필요
- **Redis 7** 실행 필요

### Docker로 실행
```bash
docker run -d --name postgres -p 5432:5432 \
  -e POSTGRES_DB=video_editor \
  -e POSTGRES_USER=admin \
  -e POSTGRES_PASSWORD=password \
  postgres:15

docker run -d --name redis -p 6379:6379 redis:7-alpine
```

## 🚀 빌드 및 실행

```bash
# 마이그레이션 실행
psql -h localhost -U admin -d video_editor -f migrations/001_initial_schema.sql

# 실행
cd frontend && npm install && npm run dev
cd backend && npm install && npm run dev
```

## ✅ 테스트

1. 브라우저 콘솔에서 "WebSocket Connected" 확인
2. 비디오 업로드 후 편집 작업 → 실시간 진행률 표시
3. "Save Project" → 프로젝트 저장
4. 새로고침 후 "Load Project" → 프로젝트 복원

## 📝 완료 기준

- [x] WebSocket 연결/재연결 동작
- [x] 처리 진행률 실시간 표시
- [x] 프로젝트 저장/불러오기 동작
- [x] Redis 캐시 동작

## 📝 다음 단계

→ `031_v2.0.0-native-addon-setup/`: C++ Native Addon 기초
