# native-video-editor

웹 브라우저에서 돌아가는 비디오 편집기.  
React 프론트 + Node.js 백엔드 + C++ 네이티브 애드온 조합.

## 왜 만들었나

Voyager X 지원 준비하면서 만든 프로젝트.  
fluent-ffmpeg가 느려서 C++ N-API로 직접 FFmpeg 붙여봤는데 진짜 빨라짐.

## 기술 스택

**프론트엔드**
- React 18, TypeScript, Vite
- WebGL2 (GPU 렌더링)
- WebAudio API

**백엔드**
- Node.js 20, Express
- PostgreSQL, Redis
- WebSocket (진행률 실시간)

**네이티브**
- C++17, N-API
- FFmpeg C API (libavformat, libavcodec, libswscale)

**인프라**
- Docker Compose
- Prometheus + Grafana

## 기능

- 비디오 업로드/재생
- Trim, Split (코덱 복사 모드)
- 자막 번인 (SRT)
- 속도 조절 (0.5x ~ 2.0x)
- 네이티브 썸네일 추출 (p99 < 50ms)
- 네이티브 메타데이터 분석 (fluent-ffmpeg 대비 50배)
- WebGL 기반 이펙트 (밝기, 대비, 채도)
- 오디오 이펙트 + 실시간 파형

## 구조

```
frontend/          # React
backend/           # Express API
native/            # C++ N-API addon
├── src/
│   ├── thumbnail/
│   ├── metadata/
│   └── bindings/
└── binding.gyp
deployments/       # Docker
monitoring/        # Prometheus, Grafana
```

## 설치

```bash
# 의존성
brew install ffmpeg  # macOS
# apt install libavformat-dev libavcodec-dev ...  # Ubuntu

# 프론트엔드
cd frontend && npm install && npm run dev

# 백엔드
cd backend && npm install && npm run dev

# 네이티브 빌드
cd native
npm install
npx node-gyp rebuild
```

## Docker로 전체 실행

```bash
docker-compose -f deployments/docker/docker-compose.yml up
```

6개 서비스 뜸: frontend, backend, native, postgres, redis, nginx

## API

```
POST /api/upload          # 비디오 업로드 (multipart)
POST /api/trim            # 구간 자르기
POST /api/split           # 분할
POST /api/subtitle        # 자막 번인
POST /api/speed           # 속도 변경
GET  /api/thumbnail/:id   # 썸네일
GET  /api/metadata/:id    # 메타데이터

WebSocket /ws             # 진행률 실시간
```

## 성능

| 항목 | fluent-ffmpeg | C++ Native |
|------|---------------|------------|
| 메타데이터 분석 | ~100ms | ~2ms |
| 썸네일 추출 | ~200ms | ~47ms |

## 환경변수

| 변수 | 기본값 |
|------|--------|
| `PORT` | 3000 |
| `DATABASE_URL` | postgres://... |
| `REDIS_URL` | redis://localhost:6379 |
| `UPLOAD_DIR` | ./uploads |

## TODO

- [ ] 멀티트랙 타임라인
- [ ] GPU 인코딩 (NVENC)
- [ ] 협업 편집 (CRDT)
- [ ] 모바일 반응형

## License

MIT
