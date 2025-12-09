// [FILE]
// - 목적: Express 백엔드 서버의 진입점 (Entry Point)
// - 주요 역할: HTTP 서버 생성, 미들웨어 설정, 라우트 등록, WebSocket 초기화
// - 관련 클론 가이드 단계: [CG-v0.1.0] 부트스트랩, [CG-v1.3.0] WebSocket
// - 권장 읽는 순서: Order 1 (import) → Order 2 (미들웨어) → Order 3 (라우트) → Order 4 (초기화)
//
// [LEARN] C 개발자를 위한 Express 이해:
// - Express는 Node.js의 HTTP 서버를 추상화한 웹 프레임워크이다.
// - C에서 소켓을 열고 HTTP 프로토콜을 직접 파싱하는 것을 Express가 대신 처리한다.
// - app.use()로 "미들웨어"를 체인으로 연결하면, 요청이 순서대로 통과한다.
// - C의 함수 호출 체인과 유사하지만, 비동기로 동작한다.

import express from 'express';
import cors from 'cors';
import http from 'http';
import uploadRoutes from './routes/upload.routes.js';
import editRoutes from './routes/edit.routes.js';
import projectRoutes from './routes/project.routes.js';
import thumbnailRoutes from './routes/thumbnail.js';
import metadataRoutes from './routes/metadata.js';
import metricsRoutes from './routes/metrics.js';
import { StorageService } from './services/storage.service.js';
import { WebSocketService } from './ws/websocket.service.js';
import { db } from './db/database.service.js';
import { redis } from './db/redis.service.js';
import { nativeVideoService } from './services/native-video.service.js';

// [Order 1] Express 앱 및 HTTP 서버 생성
const app = express();
const PORT = process.env.PORT || 3001;

// [LEARN] HTTP 서버를 별도로 생성하는 이유
// - Express 앱을 http.createServer에 전달하면, 같은 포트에서
//   HTTP와 WebSocket을 함께 처리할 수 있다.
// - WebSocket은 HTTP 핸드셰이크로 시작하므로 같은 서버를 공유한다.
const server = http.createServer(app);

const storageService = new StorageService();

// WebSocket 서비스 초기화
// - 관련 단계: [CG-v1.3.0] WebSocket 실시간 통신
const wsService = new WebSocketService(server);

// [LEARN] 전역 객체에 서비스 인스턴스 저장
// - 다른 모듈에서 wsService에 접근하기 위한 방법이다.
// - C의 전역 변수와 유사하지만, TypeScript에서는 타입 안전성을 위해
//   명시적으로 any 타입을 지정해야 한다.
// eslint-disable-next-line @typescript-eslint/no-explicit-any
(global as any).wsService = wsService;

// [Order 2] 미들웨어 설정
// [LEARN] 미들웨어란?
// - 요청(Request)과 응답(Response) 사이에서 실행되는 함수들이다.
// - C에서 main() 전에 호출되는 초기화 함수들과 유사하다.
// - 순서가 중요하며, app.use()로 등록한 순서대로 실행된다.

// CORS (Cross-Origin Resource Sharing) 허용
// - 다른 도메인(프론트엔드)에서 이 서버로 요청을 보낼 수 있게 한다.
app.use(cors());
// JSON 바디 파싱 미들웨어
// - Content-Type: application/json 요청의 바디를 자동으로 파싱한다.
app.use(express.json());

// [Order 2-1] 정적 파일 서빙
// - /videos 경로로 업로드된 비디오 파일에 접근할 수 있게 한다.
// - C에서 파일을 직접 읽어 HTTP 응답으로 보내는 것을 Express가 자동 처리한다.
app.use('/videos', express.static(storageService.getUploadDir()));

// [Order 3] 라우트 등록
// [LEARN] 라우트(Route)란?
// - HTTP 요청 경로(path)와 메서드(GET, POST 등)에 따라 실행할 핸들러를 정의한다.
// - C에서 switch문으로 명령을 분기하는 것과 유사하다.
// - app.use(경로, 라우터)로 라우터를 특정 경로에 마운트한다.

// 헬스 체크 엔드포인트
// - 서버 상태, WebSocket 연결 수, Native 모듈 상태를 반환한다.
// - 모니터링 및 로드밸런서 헬스 체크에 사용된다.
app.get('/health', (_req, res) => {
  res.json({
    status: 'ok',
    service: 'video-editor-backend',
    websocket: {
      connected: wsService.getClientCount(),
    },
    native: {
      available: nativeVideoService.isAvailable(),
      version: nativeVideoService.isAvailable() ? nativeVideoService.getVersion() : 'n/a',
    }
  });
});

// API 라우트 등록
// - /api/upload: 비디오 업로드 [CG-v1.0.0]
// - /api/edit: 트림, 분할 등 편집 [CG-v1.1.0]
// - /api/projects: 프로젝트 저장/불러오기 [CG-v1.3.0]
// - /api/thumbnail: 썸네일 추출 [CG-v2.1.0]
// - /api/metadata: 메타데이터 분석 [CG-v2.2.0]
app.use('/api', uploadRoutes);
app.use('/api/edit', editRoutes);
app.use('/api/projects', projectRoutes);
app.use('/api/thumbnail', thumbnailRoutes);
app.use('/api/metadata', metadataRoutes);

// Prometheus 메트릭 엔드포인트 [CG-v2.3.0]
app.use('/metrics', metricsRoutes);

// [Order 3-1] 에러 처리 미들웨어
// [LEARN] Express 에러 핸들러
// - 4개의 인자를 받는 미들웨어는 에러 핸들러로 인식된다.
// - 다른 미들웨어나 라우트에서 next(error)를 호출하거나 예외가 발생하면 실행된다.
app.use((err: Error, _req: express.Request, res: express.Response) => {
  console.error('Error:', err.message);
  res.status(500).json({ error: err.message || 'Internal server error' });
});

// [Order 4] 서버 초기화 및 시작
// [LEARN] async 즉시 실행 함수 (IIFE)
// - 서버 시작 전 필요한 초기화 작업을 비동기로 수행한다.
// - C에서 main() 시작 부분의 초기화 코드와 유사하다.
async function initialize() {
  // 업로드 디렉터리 생성
  await storageService.ensureUploadDir();
  console.log('✓ Upload directory initialized');

  // 데이터베이스 마이그레이션 실행
  // - 관련 단계: [CG-v1.3.0] PostgreSQL 연동
  try {
    await db.runMigrations();
    console.log('✓ Database migrations completed');
  } catch (error) {
    console.error('⚠ Database migrations failed:', error);
    console.log('  Continuing without database (for development)');
  }
}

initialize().catch(console.error);

// [Order 4-1] Graceful Shutdown
// [LEARN] 시그널 처리
// - SIGTERM: 프로세스 종료 요청 시그널 (Docker, Kubernetes에서 사용)
// - C에서 signal()로 시그널 핸들러를 등록하는 것과 동일한 목적이다.
// - 연결된 클라이언트들을 정리하고 안전하게 종료한다.
process.on('SIGTERM', async () => {
  console.log('SIGTERM received, closing connections...');
  // 리소스 정리: WebSocket, DB, Redis 연결 종료
  wsService.close();
  await db.close();
  await redis.close();
  server.close(() => {
    console.log('Server closed');
    process.exit(0);
  });
});

// [Order 5] HTTP 서버 시작
// - listen()으로 지정된 포트에서 연결 대기를 시작한다.
// - C에서 bind() + listen() + accept() 루프와 유사하다.
server.listen(PORT, () => {
  console.log(`native-video-editor Backend running on port ${PORT}`);
  console.log(`Upload directory: ${storageService.getUploadDir()}`);
  console.log(`WebSocket server ready at ws://localhost:${PORT}/ws`);
});

// [Reader Notes]
// =============================================================================
// 이 파일에서 처음 등장한 개념: Express 미들웨어, 라우터, HTTP 서버
//
// ## Express 아키텍처 (C 개발자 관점)
// 1. **미들웨어 체인**: 함수 포인터 배열을 순회하며 호출하는 것과 유사
//    요청 → cors → json parser → 라우터 → 에러 핸들러 → 응답
//
// 2. **라우터**: URL 경로별 분기 처리 (switch-case와 유사)
//    /api/upload → uploadRoutes
//    /api/edit → editRoutes
//
// 3. **서비스 레이어**: 비즈니스 로직을 분리 (C의 모듈 분리와 동일)
//    routes → services → 외부 시스템 (FFmpeg, DB, 파일 시스템)
//
// ## Node.js 이벤트 루프 (C 개발자 관점)
// - C의 select/poll/epoll과 유사한 이벤트 기반 I/O
// - 단일 스레드로 동작하지만, I/O 작업은 백그라운드에서 처리
// - 콜백/Promise/async-await로 비동기 코드 작성
//
// ## 이 파일을 이해한 다음, 이어서 보면 좋은 파일:
// 1. routes/upload.routes.ts - 업로드 API 엔드포인트
// 2. services/ffmpeg.service.ts - FFmpeg 호출 서비스
// 3. ws/websocket.service.ts - WebSocket 서버 구현
// =============================================================================
