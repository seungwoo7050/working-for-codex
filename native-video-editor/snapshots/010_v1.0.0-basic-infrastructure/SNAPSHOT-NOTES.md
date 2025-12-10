# 010_v1.0.0-basic-infrastructure 스냅샷

## 📋 패치 정보

| 항목 | 값 |
|------|-----|
| 패치 범위 | 1.0.1 ~ 1.0.6 |
| 버전 태그 | v1.0.0 |
| 이름 | Basic Infrastructure |
| 이전 단계 | v0.1.0 (Bootstrap) |

## 🎯 이 스냅샷의 목표

비디오 업로드, 재생, 타임라인 UI의 기본 인프라를 구축합니다.

### 추가된 기능
- **VideoUpload 컴포넌트**: 드래그 앤 드롭 + 파일 선택 업로드
- **useVideoUpload 훅**: 업로드 상태 관리 (진행률, 에러)
- **multer 라우트**: 서버 측 파일 업로드 처리
- **VideoPlayer 컴포넌트**: HTML5 Video API 기반 재생
- **Timeline 컴포넌트**: Canvas 기반 시간 눈금자
- **App.tsx 통합**: 모든 컴포넌트 연결

## 📁 디렉토리 구조

```
010_v1.0.0-basic-infrastructure/
├── frontend/
│   ├── package.json
│   ├── vite.config.ts
│   ├── tsconfig.json
│   └── src/
│       ├── App.tsx
│       ├── main.tsx
│       ├── index.css
│       ├── components/
│       │   ├── VideoUpload.tsx
│       │   ├── VideoPlayer.tsx
│       │   └── Timeline.tsx
│       ├── hooks/
│       │   └── useVideoUpload.ts
│       └── types/
│           └── video.ts
├── backend/
│   ├── package.json
│   ├── tsconfig.json
│   └── src/
│       ├── server.ts
│       └── routes/
│           └── upload.routes.ts
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

### 3. 테스트

1. http://localhost:5173 접속
2. 비디오 파일을 드래그 앤 드롭 또는 클릭하여 업로드
3. 업로드 완료 후 비디오 재생 확인
4. 타임라인에서 시간 클릭하여 seek 확인

## ✅ 완료 기준

- [x] 100MB 비디오 업로드 → URL 반환
- [x] 비디오 재생/일시정지/시크 동작
- [x] Canvas 타임라인 렌더링
- [x] 콘솔 에러 0

## 📝 이전 버전 대비 변경사항

### 추가된 파일
- `frontend/src/components/VideoUpload.tsx`
- `frontend/src/components/VideoPlayer.tsx`
- `frontend/src/components/Timeline.tsx`
- `frontend/src/hooks/useVideoUpload.ts`
- `frontend/src/types/video.ts`
- `backend/src/routes/upload.routes.ts`

### 수정된 파일
- `frontend/package.json` (의존성 추가)
- `backend/package.json` (multer 추가)
- `frontend/src/App.tsx` (컴포넌트 통합)
- `backend/src/server.ts` (라우트 연결)

## 📝 다음 단계

→ `015_v1.1.0-trim-split/`: Trim & Split 편집 기능 추가
