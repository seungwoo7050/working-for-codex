# 015_v1.1.0-trim-split 스냅샷

## 📋 패치 정보

| 항목 | 값 |
|------|-----|
| 패치 범위 | 1.1.1 ~ 1.1.5 |
| 버전 태그 | v1.1.0 |
| 이름 | Trim & Split |
| 이전 단계 | v1.0.0 (Basic Infrastructure) |

## 🎯 이 스냅샷의 목표

비디오 트림(구간 추출) 및 분할 기능을 구현합니다.

### 추가된 기능
- **StorageService**: 파일 저장/조회/삭제 관리
- **FFmpegService**: fluent-ffmpeg 기반 trim/split 처리
- **edit.routes.ts**: `/api/trim`, `/api/split` API 엔드포인트
- **EditPanel 컴포넌트**: 트림/분할 UI
- **useVideoEdit 훅**: 편집 작업 상태 관리

## 📁 포함되어야 할 파일

### v1.0.0에서 상속
- 모든 v1.0.0 파일들

### 신규 추가 파일
```
frontend/src/
├── components/
│   └── EditPanel.tsx          # 트림/분할 UI
├── hooks/
│   └── useVideoEdit.ts        # 편집 상태 관리
└── types/
    └── edit.ts                # 편집 타입 정의

backend/src/
├── services/
│   ├── storage.service.ts     # 파일 관리
│   └── ffmpeg.service.ts      # FFmpeg 래퍼
├── routes/
│   └── edit.routes.ts         # 편집 API
└── outputs/                   # 출력 디렉토리
```

## 🔧 추가 요구사항

- **FFmpeg 설치 필요**
  - macOS: `brew install ffmpeg`
  - Linux: `apt install ffmpeg`

## 🚀 빌드 및 실행

```bash
# 의존성 설치
cd frontend && npm install
cd ../backend && npm install

# 실행
cd frontend && npm run dev  # Terminal 1
cd backend && npm run dev   # Terminal 2
```

## ✅ 테스트

1. 비디오 업로드
2. EditPanel에서 시작/종료 시간 입력
3. "Trim" 클릭 → 구간 추출 결과 확인
4. "Split" 클릭 → 2개 파일 생성 확인

## 📝 완료 기준

- [x] Trim: 1분 영상 10s-30s 구간 추출 → 20s 출력
- [x] Split: 30s 지점에서 분할 → 2개 파일 생성
- [x] 처리 시간 < 5s (100MB, 코덱 카피)

## 📝 다음 단계

→ `019_v1.2.0-subtitle-speed/`: 자막 추가 및 속도 조절 기능
