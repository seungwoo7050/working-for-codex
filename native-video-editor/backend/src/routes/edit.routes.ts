// [FILE]
// - 목적: 비디오 편집 API 엔드포인트 (트림, 분할, 자막, 속도)
// - 주요 역할: 요청 검증, FFmpegService 호출, 결과 반환
// - 관련 클론 가이드 단계: [CG-v1.1.0] Trim/Split, [CG-v1.2.0] 자막/속도
// - 권장 읽는 순서: Order 1 (트림) → Order 2 (분할) → Order 3 (자막/속도)
//
// [LEARN] C 개발자를 위한 REST API 설계:
// - HTTP 메서드와 URL로 작업 종류를 표현한다.
// - POST /api/edit/trim: 트림 작업 생성
// - POST /api/edit/split: 분할 작업 생성
// - 요청 바디에 JSON으로 파라미터를 전달한다.

import { Router, Request, Response } from 'express';
import { FFmpegService } from '../services/ffmpeg.service.js';
import { StorageService } from '../services/storage.service.js';

const router = Router();
const storageService = new StorageService();
const ffmpegService = new FFmpegService(storageService);

// [Order 1] 트림(Trim) 엔드포인트
// - 비디오의 특정 구간만 추출한다.
/**
 * Trim video from start time to end time
 * POST /api/edit/trim
 * Body: { filename: string, startTime: number, endTime: number }
 */
router.post('/trim', async (req: Request, res: Response): Promise<void> => {
  try {
    // [LEARN] 요청 바디 파싱
    // - Express의 express.json() 미들웨어가 JSON을 자동으로 파싱한다.
    // - req.body에서 필요한 필드를 구조 분해 할당으로 추출한다.
    const { filename, startTime, endTime } = req.body;

    // [LEARN] 입력 검증 (Input Validation)
    // - 클라이언트 입력을 신뢰하지 않고 서버에서 검증한다.
    // - C에서 함수 시작 부분에서 인자를 검증하는 것과 동일한 패턴이다.
    if (!filename || startTime === undefined || endTime === undefined) {
      res.status(400).json({
        error: 'Missing required fields: filename, startTime, endTime',
      });
      return;
    }

    if (startTime < 0 || endTime <= startTime) {
      res.status(400).json({
        error: 'Invalid time range: endTime must be greater than startTime',
      });
      return;
    }

    // 파일 존재 확인
    const inputPath = storageService.getFilePath(filename);
    const fileExists = await storageService.fileExists(filename);

    if (!fileExists) {
      res.status(404).json({ error: 'Video file not found' });
      return;
    }

    // FFmpeg 서비스 호출
    const result = await ffmpegService.trimVideo(
      inputPath,
      startTime,
      undefined,
      endTime
    );

    // Include API URL in response
    const apiUrl = process.env.API_URL || 'http://localhost:3001';
    res.json({
      ...result,
      url: `${apiUrl}${result.url}`,
    });
  } catch (error) {
    console.error('Trim error:', error);
    const errorMessage = error instanceof Error ? error.message : 'Failed to trim video';
    res.status(500).json({ error: errorMessage });
  }
});

// [Order 2] 분할(Split) 엔드포인트
// - 비디오를 특정 시점에서 두 개의 파일로 분할한다.
/**
 * Split video at a specific time into two files
 * POST /api/edit/split
 * Body: { filename: string, splitTime: number }
 */
router.post('/split', async (req: Request, res: Response): Promise<void> => {
  try {
    const { filename, splitTime } = req.body;

    // [LEARN] 필수 필드 검증
    // - undefined 체크로 필수 파라미터 누락을 감지한다.
    if (!filename || splitTime === undefined) {
      res.status(400).json({
        error: 'Missing required fields: filename, splitTime',
      });
      return;
    }

    // [LEARN] 비즈니스 규칙 검증
    // - splitTime이 0 이하면 의미 없는 분할이므로 거부한다.
    if (splitTime <= 0) {
      res.status(400).json({
        error: 'Invalid split time: must be greater than 0',
      });
      return;
    }

    const inputPath = storageService.getFilePath(filename);
    const fileExists = await storageService.fileExists(filename);

    if (!fileExists) {
      res.status(404).json({ error: 'Video file not found' });
      return;
    }

    // [LEARN] 분할 결과는 배열로 반환된다 (앞부분, 뒷부분)
    const results = await ffmpegService.splitVideo(inputPath, splitTime);

    // [LEARN] 응답 데이터 가공
    // - map()으로 각 결과에 전체 URL을 추가한다.
    // - 스프레드 연산자(...result)로 기존 필드를 복사한다.
    // Include API URL in response
    const apiUrl = process.env.API_URL || 'http://localhost:3001';
    res.json({
      parts: results.map((result) => ({
        ...result,
        url: `${apiUrl}${result.url}`,
      })),
    });
  } catch (error) {
    console.error('Split error:', error);
    const errorMessage = error instanceof Error ? error.message : 'Failed to split video';
    res.status(500).json({ error: errorMessage });
  }
});

// [Order 3] 메타데이터 조회 엔드포인트
// - 비디오 파일의 정보(해상도, 길이, 코덱 등)를 조회한다.
/**
 * Get video metadata
 * GET /api/edit/metadata/:filename
 */
// [LEARN] URL 파라미터
// - :filename은 URL 경로의 일부로 전달된다.
// - req.params.filename으로 접근한다.
// - 쿼리 스트링(?key=value)과 달리 경로에 포함되어 RESTful하다.
router.get('/metadata/:filename', async (req: Request, res: Response): Promise<void> => {
  try {
    const { filename } = req.params;

    const inputPath = storageService.getFilePath(filename);
    const fileExists = await storageService.fileExists(filename);

    if (!fileExists) {
      res.status(404).json({ error: 'Video file not found' });
      return;
    }

    const metadata = await ffmpegService.getVideoMetadata(inputPath);
    res.json(metadata);
  } catch (error) {
    console.error('Metadata error:', error);
    const errorMessage = error instanceof Error ? error.message : 'Failed to get metadata';
    res.status(500).json({ error: errorMessage });
  }
});

// [Order 4] 자막/속도 처리 엔드포인트
// - 자막 추가와 재생 속도 변경을 동시에 처리한다.
/**
 * Process video with subtitles and/or speed change
 * POST /api/edit/process
 * Body: { filename: string, subtitles?: Subtitle[], speed?: number }
 */
router.post('/process', async (req: Request, res: Response): Promise<void> => {
  try {
    const { filename, subtitles, speed } = req.body;

    if (!filename) {
      res.status(400).json({ error: 'Missing required field: filename' });
      return;
    }

    // [LEARN] 상호 의존 검증
    // - subtitles와 speed 중 최소 하나는 있어야 의미 있는 요청이다.
    if (!subtitles && !speed) {
      res.status(400).json({
        error: 'At least one of subtitles or speed must be provided',
      });
      return;
    }

    // [LEARN] 범위 검증
    // - 속도는 0.5배~2.0배로 제한한다 (FFmpeg 안정성 및 UX 고려).
    if (speed && (speed < 0.5 || speed > 2.0)) {
      res.status(400).json({
        error: 'Speed must be between 0.5 and 2.0',
      });
      return;
    }

    const inputPath = storageService.getFilePath(filename);
    const fileExists = await storageService.fileExists(filename);

    if (!fileExists) {
      res.status(404).json({ error: 'Video file not found' });
      return;
    }

    // [LEARN] 기본값 처리
    // - subtitles가 undefined면 빈 배열을 전달한다.
    // - speed가 undefined면 FFmpegService에서 기본값(1.0)을 사용한다.
    const result = await ffmpegService.addSubtitlesAndSpeed(
      inputPath,
      subtitles || [],
      speed
    );

    // Include API URL in response
    const apiUrl = process.env.API_URL || 'http://localhost:3001';
    res.json({
      ...result,
      url: `${apiUrl}${result.url}`,
    });
  } catch (error) {
    console.error('Process error:', error);
    const errorMessage = error instanceof Error ? error.message : 'Failed to process video';
    res.status(500).json({ error: errorMessage });
  }
});

export default router;
