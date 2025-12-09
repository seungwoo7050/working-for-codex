# 004_v0.1.0-bootstrap 스냅샷

## 📋 패치 정보

| 항목 | 값 |
|------|-----|
| 패치 범위 | 0.1 ~ 0.4 |
| 버전 태그 | v0.1.0 |
| 이름 | Bootstrap |
| 이전 단계 | (없음 - 시작점) |

## 🎯 이 스냅샷의 목표

프로젝트의 기본 구조를 설정하고 개발 환경을 구축합니다.

- 모노레포 디렉토리 구조 생성
- Vite + React 18 + TypeScript 프론트엔드 설정
- Express + TypeScript 백엔드 설정
- 로컬 개발 환경 검증

## 📁 디렉토리 구조

```
004_v0.1.0-bootstrap/
├── frontend/
│   ├── package.json
│   ├── vite.config.ts
│   ├── tsconfig.json
│   ├── tsconfig.node.json
│   ├── postcss.config.js
│   ├── tailwind.config.js
│   ├── index.html
│   └── src/
│       ├── App.tsx
│       ├── main.tsx
│       ├── index.css
│       └── vite-env.d.ts
├── backend/
│   ├── package.json
│   ├── tsconfig.json
│   └── src/
│       └── server.ts
└── SNAPSHOT-NOTES.md
```

## 🚀 빌드 및 실행

### 1. 의존성 설치

```bash
# Frontend
cd frontend && npm install

# Backend
cd ../backend && npm install
```

### 2. 개발 서버 실행

```bash
# Terminal 1: Frontend
cd frontend && npm run dev

# Terminal 2: Backend
cd backend && npm run dev
```

### 3. 확인

- Frontend: http://localhost:5173 (Hello World 페이지)
- Backend: http://localhost:3001/health (JSON 응답)

## ✅ 완료 기준

- [x] Frontend Vite 개발 서버 실행
- [x] Backend Express 서버 실행
- [x] TypeScript 컴파일 에러 0
- [x] Hot reload 동작

## 📝 다음 단계

→ `010_v1.0.0-basic-infrastructure/`: 비디오 업로드/재생 기능 추가
