// [FILE]
// - 목적: 비디오 업로드 API 엔드포인트 정의
// - 주요 역할: multer로 멀티파트 파일 업로드 처리, 파일 저장, 응답 반환
// - 관련 클론 가이드 단계: [CG-v1.0.0] 기본 인프라 - 비디오 업로드
// - 권장 읽는 순서: Order 1 (multer 설정) → Order 2 (업로드 핸들러)
//
// [LEARN] C 개발자를 위한 multer 이해:
// - multer는 multipart/form-data 파싱 미들웨어이다.
// - C에서 HTTP 바디를 직접 파싱하여 파일 경계(boundary)를 찾고
//   청크 단위로 파일에 쓰는 작업을 multer가 자동으로 처리한다.
// - diskStorage: 파일을 디스크에 저장 (vs. memoryStorage: 메모리에 버퍼로 저장)

import { Router, Request, Response } from 'express';
import multer from 'multer';
import path from 'path';
import { StorageService } from '../services/storage.service.js';

const router = Router();
const storageService = new StorageService();

// [Order 1] multer 스토리지 설정
// [LEARN] diskStorage 콜백 패턴
// - destination: 파일을 저장할 디렉터리 결정
// - filename: 저장할 파일명 결정
// - 콜백 함수 cb(에러, 값)로 결과를 전달 (Node.js 콜백 관례)
const storage = multer.diskStorage({
  destination: async (_req, _file, cb) => {
    await storageService.ensureUploadDir();
    cb(null, storageService.getUploadDir());
  },
  filename: (_req, file, cb) => {
    // 고유한 파일명 생성: 타임스탬프 + 랜덤 숫자 + 원본 확장자
    const uniqueSuffix = Date.now() + '-' + Math.round(Math.random() * 1e9);
    const ext = path.extname(file.originalname);
    cb(null, 'video-' + uniqueSuffix + ext);
  },
});

// [Order 1-1] multer 인스턴스 생성
// - storage: 스토리지 엔진 설정
// - limits: 파일 크기 제한 (500MB)
// - fileFilter: 허용할 MIME 타입 검증
const upload = multer({
  storage,
  limits: {
    fileSize: 500 * 1024 * 1024, // 500MB max file size
  },
  fileFilter: (_req, file, cb) => {
    // [LEARN] MIME 타입 검증
    // - 클라이언트가 보낸 Content-Type을 신뢰하지 않고 서버에서 검증
    // - 보안상 중요: 악의적인 파일 업로드 방지
    const allowedMimes = [
      'video/mp4',
      'video/mpeg',
      'video/quicktime',
      'video/x-msvideo',
      'video/x-matroska',
      'video/webm',
    ];
    if (allowedMimes.includes(file.mimetype)) {
      cb(null, true);
    } else {
      cb(new Error('Invalid file type. Only video files are allowed.'));
    }
  },
});

// [Order 2] 업로드 엔드포인트
// [LEARN] Express 라우트 핸들러 구조
// - router.post(경로, 미들웨어들..., 핸들러)
// - upload.single('video'): 'video' 필드로 전송된 단일 파일을 처리하는 미들웨어
// - 미들웨어가 성공하면 req.file에 파일 정보가 담긴다.
/**
 * Upload video file
 * POST /api/upload
 */
router.post('/upload', upload.single('video'), (req: Request, res: Response): void => {
  try {
    // [LEARN] 방어적 프로그래밍
    // - multer 미들웨어가 실패하거나 파일이 없는 경우를 처리
    if (!req.file) {
      res.status(400).json({ error: 'No video file provided' });
      return;
    }

    // 응답 객체 구성
    // - url: 프론트엔드에서 비디오에 접근할 경로
    // - path: 서버 파일 시스템 경로
    // - filename: 저장된 파일명
    // - originalName: 클라이언트가 보낸 원본 파일명
    // - size: 파일 크기 (바이트)
    // - mimetype: MIME 타입
    const videoUrl = `/videos/${req.file.filename}`;
    const response = {
      url: videoUrl,
      path: req.file.path,
      filename: req.file.filename,
      originalName: req.file.originalname,
      size: req.file.size,
      mimetype: req.file.mimetype,
    };

    res.json(response);
  } catch (error) {
    console.error('Upload error:', error);
    res.status(500).json({ error: 'Failed to upload video' });
  }
});

export default router;

// [Reader Notes]
// =============================================================================
// 이 파일에서 처음 등장한 개념: Express Router, multer, 미들웨어 체인
//
// ## Express Router (C 개발자 관점)
// - Router()는 미니 앱처럼 동작하는 라우트 그룹이다.
// - C에서 함수 포인터 테이블로 명령을 분기하는 것과 유사하다.
// - server.ts에서 app.use('/api', uploadRoutes)로 마운트된다.
//
// ## multer 미들웨어 체인
// 1. 요청 수신 → multer가 multipart 데이터 파싱
// 2. 파일을 디스크에 저장 → req.file에 정보 저장
// 3. 다음 미들웨어(핸들러)로 전달
//
// ## 파일 업로드 보안 고려사항
// - MIME 타입 검증 (fileFilter)
// - 파일 크기 제한 (limits.fileSize)
// - 파일명 변경 (원본 파일명 사용 금지)
// - 실행 권한 제거 (운영체제 설정)
//
// ## 이 파일을 이해한 다음, 이어서 보면 좋은 파일:
// 1. routes/edit.routes.ts - 편집 API 엔드포인트
// 2. services/storage.service.ts - 파일 저장소 서비스
// =============================================================================
