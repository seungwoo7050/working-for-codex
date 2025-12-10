# 036_v2.1.0-thumbnail-extraction 스냅샷

## 📋 패치 정보

| 항목 | 값 |
|------|-----|
| 패치 범위 | 2.1.1 ~ 2.1.5 |
| 버전 태그 | v2.1.0 |
| 이름 | Thumbnail Extraction |
| 이전 단계 | v2.0.0 (Native Addon Setup) |

## 🎯 이 스냅샷의 목표

FFmpeg C API를 사용한 고성능 썸네일 추출 기능을 구현합니다.

### 추가된 기능
- **ThumbnailExtractor 클래스**: C++ 썸네일 추출기
  - `open_video()`: 비디오 파일 열기
  - `seek_and_decode()`: 특정 시간으로 seek 후 디코딩
  - `frame_to_jpeg()`: 프레임을 JPEG로 변환
- **N-API ObjectWrap 바인딩**: JavaScript에서 C++ 클래스 사용
- **thumbnail.ts 라우트**: `/api/thumbnail` 엔드포인트

## 📁 포함되어야 할 파일

### v2.0.0에서 상속
- 모든 v2.0.0 파일들

### 신규 추가 파일
```
native/
├── include/
│   └── thumbnail_extractor.h   # 썸네일 추출기 헤더
└── src/
    └── thumbnail_extractor.cpp # 썸네일 추출기 구현

backend/src/
├── routes/
│   └── thumbnail.ts            # 썸네일 API
└── services/
    └── native-video.service.ts # Native 모듈 래퍼
```

## 🚀 빌드 및 실행

```bash
# Native 모듈 재빌드
cd native && npm run build

# Backend 실행
cd ../backend && npm run dev

# API 테스트
curl "http://localhost:3001/api/thumbnail?path=uploads/test.mp4&timestamp=5.0"
```

## ✅ 테스트

1. 비디오 업로드
2. `/api/thumbnail?timestamp=5.0` 호출
3. 50ms 이내 응답 확인

## 📝 완료 기준

- [x] 썸네일 추출 p99 < 50ms
- [x] 메모리 풀 재사용 동작
- [x] 손상된 비디오 graceful 에러 처리

## 📝 다음 단계

→ `039_v2.2.0-metadata-analysis/`: 메타데이터 분석 기능
