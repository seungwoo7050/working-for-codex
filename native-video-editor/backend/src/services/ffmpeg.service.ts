// [FILE]
// - 목적: FFmpeg를 이용한 비디오 처리 서비스
// - 주요 역할: 트림, 분할, 자막 추가, 속도 변경, 메타데이터 추출 등
// - 관련 클론 가이드 단계: [CG-v1.1.0] Trim/Split, [CG-v1.2.0] 자막/속도
// - 권장 읽는 순서: Order 1 (타입 정의) → Order 2 (클래스 구조) → Order 3 (주요 메서드)
//
// [LEARN] C 개발자를 위한 FFmpeg 이해:
// - FFmpeg는 비디오/오디오 처리를 위한 C 라이브러리 및 CLI 도구이다.
// - fluent-ffmpeg는 Node.js에서 FFmpeg CLI를 래핑한 라이브러리이다.
// - C에서 fork() + exec()로 외부 프로세스를 실행하는 것과 유사하다.
// - Phase 2에서는 C++ Native Addon으로 FFmpeg C API를 직접 호출한다.

import ffmpeg from 'fluent-ffmpeg';
import path from 'path';
import { promises as fs } from 'fs';
import { StorageService } from './storage.service.js';

// [Order 1] 타입 정의
/**
 * Result of a video processing operation
 */
export interface ProcessingResult {
  filename: string;
  path: string;
  url: string;
  size: number;
  duration?: number;
}

/**
 * Subtitle definition
 */
export interface Subtitle {
  text: string;
  startTime: number;
  duration: number;
}

// [Order 2] FFmpeg 서비스 클래스
// [LEARN] 클래스 기반 서비스 패턴
// - 관련 기능들을 하나의 클래스로 그룹화한다.
// - C에서 구조체 + 함수 포인터로 모듈을 구성하는 것과 유사하다.
// - 의존성 주입(DI): 생성자에서 필요한 서비스를 받는다.
/**
 * FFmpeg service for video processing operations
 */
export class FFmpegService {
  private storageService: StorageService;

  constructor(storageService: StorageService) {
    this.storageService = storageService;
  }

  /**
   * Get WebSocket service for progress broadcasting
   */
  private getWsService() {
    // 전역 객체에서 WebSocket 서비스 인스턴스를 가져온다.
    // eslint-disable-next-line @typescript-eslint/no-explicit-any
    return (global as any).wsService;
  }

  /**
   * Emit progress via WebSocket
   */
  private emitProgress(operationId: string, operation: string, progress: number, message?: string) {
    // [LEARN] 실시간 진행률 브로드캐스트
    // - 긴 작업(트림, 인코딩 등) 중 클라이언트에게 진행률을 전송한다.
    // - 관련 단계: [CG-v1.3.0] WebSocket 실시간 통신
    const ws = this.getWsService();
    if (ws) {
      ws.broadcastProgress({
        operationId,
        operation,
        progress,
        message,
      });
    }
  }

  // [Order 3] 주요 비디오 처리 메서드

  // [Order 3-1] 트림(Trim) - 비디오 자르기
  /**
   * Trim video from start time to end time
   * @param inputPath - Path to input video file
   * @param startTime - Start time in seconds
   * @param duration - Duration in seconds (or end time if endTime provided)
   * @param endTime - End time in seconds (optional, if provided duration is ignored)
   * @returns Processing result with output file info
   */
  async trimVideo(
    inputPath: string,
    startTime: number,
    duration?: number,
    endTime?: number
  ): Promise<ProcessingResult> {
    const outputFilename = `trim-${Date.now()}-${Math.round(Math.random() * 1e9)}${path.extname(inputPath)}`;
    const outputPath = this.storageService.getFilePath(outputFilename);

    const actualDuration = endTime !== undefined ? endTime - startTime : duration;

    if (!actualDuration || actualDuration <= 0) {
      throw new Error('Invalid duration or end time');
    }

    // [LEARN] fluent-ffmpeg 체이닝 패턴
    // - ffmpeg(입력파일)로 명령 빌더를 시작한다.
    // - 메서드 체이닝으로 옵션을 추가한다.
    // - C의 ffmpeg CLI 명령어와 매핑:
    //   ffmpeg -i input.mp4 -ss 10 -t 30 -c copy output.mp4
    return new Promise((resolve, reject) => {
      let command = ffmpeg(inputPath)
        .setStartTime(startTime)    // -ss: 시작 시간
        .setDuration(actualDuration) // -t: 지속 시간
        .output(outputPath);

      // [LEARN] 코덱 복사 (Stream Copy)
      // - videoCodec('copy'): 비디오 스트림을 재인코딩 없이 복사
      // - 재인코딩 없이 복사하면 매우 빠르지만, 키프레임 정확도가 떨어질 수 있다.
      // - C FFmpeg: -c:v copy -c:a copy
      command = command
        .videoCodec('copy')
        .audioCodec('copy');

      command
        // [LEARN] 이벤트 기반 비동기 처리
        // - 'end': 처리 완료 시 호출
        // - 'error': 에러 발생 시 호출
        // - 'progress': 진행률 업데이트 시 호출 (여기서는 미사용)
        .on('end', async () => {
          try {
            const stats = await fs.stat(outputPath);
            resolve({
              filename: outputFilename,
              path: outputPath,
              url: `/videos/${outputFilename}`,
              size: stats.size,
            });
          } catch (error) {
            reject(error);
          }
        })
        .on('error', (err) => {
          reject(new Error(`FFmpeg trim error: ${err.message}`));
        })
        .run();
    });
  }

  // [Order 3-2] 분할(Split) - 비디오를 특정 시점에서 두 파트로 분할
  /**
   * Split video at a specific time into two files
   * @param inputPath - Path to input video file
   * @param splitTime - Time in seconds to split the video
   * @returns Array of processing results for both parts
   */
  async splitVideo(inputPath: string, splitTime: number): Promise<ProcessingResult[]> {
    const timestamp = Date.now();
    const random = Math.round(Math.random() * 1e9);
    const ext = path.extname(inputPath);

    const part1Filename = `split-part1-${timestamp}-${random}${ext}`;
    const part2Filename = `split-part2-${timestamp}-${random}${ext}`;

    const part1Path = this.storageService.getFilePath(part1Filename);
    const part2Path = this.storageService.getFilePath(part2Filename);

    // Get video duration first
    const metadata = await this.getVideoMetadata(inputPath);
    const totalDuration = metadata.duration;

    if (!totalDuration || splitTime >= totalDuration) {
      throw new Error('Invalid split time');
    }

    // [LEARN] 병렬 처리 패턴
    // - 두 파트를 동시에 생성하여 처리 시간을 단축한다.
    // - Promise.all()로 두 Promise가 모두 완료될 때까지 대기한다.
    // - C에서 pthread로 병렬 처리하는 것과 유사한 목적이다.

    // Part 1 생성 (0 ~ splitTime)
    const part1Promise = new Promise<ProcessingResult>((resolve, reject) => {
      ffmpeg(inputPath)
        .setStartTime(0)
        .setDuration(splitTime)
        .videoCodec('copy')
        .audioCodec('copy')
        .output(part1Path)
        .on('end', async () => {
          try {
            const stats = await fs.stat(part1Path);
            resolve({
              filename: part1Filename,
              path: part1Path,
              url: `/videos/${part1Filename}`,
              size: stats.size,
              duration: splitTime,
            });
          } catch (error) {
            reject(error);
          }
        })
        .on('error', (err) => {
          reject(new Error(`FFmpeg split part1 error: ${err.message}`));
        })
        .run();
    });

    // Part 2 생성 (splitTime ~ 끝)
    const part2Promise = new Promise<ProcessingResult>((resolve, reject) => {
      ffmpeg(inputPath)
        .setStartTime(splitTime)
        .videoCodec('copy')
        .audioCodec('copy')
        .output(part2Path)
        .on('end', async () => {
          try {
            const stats = await fs.stat(part2Path);
            resolve({
              filename: part2Filename,
              path: part2Path,
              url: `/videos/${part2Filename}`,
              size: stats.size,
              duration: totalDuration - splitTime,
            });
          } catch (error) {
            reject(error);
          }
        })
        .on('error', (err) => {
          reject(new Error(`FFmpeg split part2 error: ${err.message}`));
        })
        .run();
    });

    return Promise.all([part1Promise, part2Promise]);
  }

  // [Order 3-3] 메타데이터 추출
  // [LEARN] ffprobe란?
  // - FFmpeg에 포함된 미디어 분석 도구이다.
  // - C FFmpeg: ffprobe -v quiet -print_format json -show_format -show_streams input.mp4
  /**
   * Get video metadata including duration
   * @param videoPath - Path to video file
   * @returns Metadata object with duration, resolution, codec, etc.
   */
  async getVideoMetadata(videoPath: string): Promise<{
    duration: number;
    width?: number;
    height?: number;
    codec?: string;
    bitrate?: number;
  }> {
    return new Promise((resolve, reject) => {
      ffmpeg.ffprobe(videoPath, (err, metadata) => {
        if (err) {
          reject(new Error(`FFprobe error: ${err.message}`));
          return;
        }

        // 비디오 스트림 찾기 (오디오 스트림과 구분)
        const videoStream = metadata.streams.find((s) => s.codec_type === 'video');

        resolve({
          duration: metadata.format.duration || 0,
          width: videoStream?.width,
          height: videoStream?.height,
          codec: videoStream?.codec_name,
          bitrate: metadata.format.bit_rate,
        });
      });
    });
  }

  // [Order 3-4] 자막 추가 및 속도 변경
  // - 관련 단계: [CG-v1.2.0] 자막/속도 기능
  // [LEARN] FFmpeg 필터 그래프
  // - 비디오/오디오 스트림에 효과를 적용하는 방법이다.
  // - setpts: 비디오 속도 변경 (Presentation Timestamp 조정)
  // - subtitles: 자막을 비디오에 굽는다 (하드코딩)
  // - atempo: 오디오 속도 변경
  /**
   * Add subtitles to video and/or change speed
   * @param inputPath - Path to input video file
   * @param subtitles - Array of subtitles to add
   * @param speed - Playback speed (0.5 to 2.0)
   * @returns Processing result with output file info
   */
  async addSubtitlesAndSpeed(
    inputPath: string,
    subtitles: Subtitle[],
    speed?: number
  ): Promise<ProcessingResult> {
    const outputFilename = `processed-${Date.now()}-${Math.round(Math.random() * 1e9)}${path.extname(inputPath)}`;
    const outputPath = this.storageService.getFilePath(outputFilename);

    // Create SRT subtitle file if subtitles provided
    let subtitlePath: string | null = null;
    if (subtitles.length > 0) {
      subtitlePath = this.storageService.getFilePath(`temp-${Date.now()}.srt`);
      const srtContent = this.generateSRT(subtitles);
      await fs.writeFile(subtitlePath, srtContent, 'utf-8');
    }

    return new Promise((resolve, reject) => {
      let command = ffmpeg(inputPath);

      // Apply speed filter if specified
      const filters: string[] = [];
      if (speed && speed !== 1.0) {
        // Video speed filter
        filters.push(`setpts=${1 / speed}*PTS`);
      }

      // Add subtitle filter if subtitles provided
      if (subtitlePath) {
        // Burn subtitles into video
        const escapedPath = subtitlePath.replace(/\\/g, '/').replace(/:/g, '\\:');
        filters.push(`subtitles='${escapedPath}':force_style='Alignment=2,MarginV=20,FontSize=24'`);
      }

      if (filters.length > 0) {
        command = command.videoFilters(filters);
      }

      // Apply audio speed if specified
      if (speed && speed !== 1.0) {
        command = command.audioFilters([`atempo=${speed}`]);
      }

      command
        .output(outputPath)
        .on('end', async () => {
          // Clean up temp subtitle file
          if (subtitlePath) {
            try {
              await fs.unlink(subtitlePath);
            } catch {
              // Ignore cleanup errors
            }
          }

          try {
            const stats = await fs.stat(outputPath);
            resolve({
              filename: outputFilename,
              path: outputPath,
              url: `/videos/${outputFilename}`,
              size: stats.size,
            });
          } catch (error) {
            reject(error);
          }
        })
        .on('error', async (err) => {
          // Clean up temp subtitle file on error
          if (subtitlePath) {
            try {
              await fs.unlink(subtitlePath);
            } catch {
              // Ignore cleanup errors
            }
          }
          reject(new Error(`FFmpeg processing error: ${err.message}`));
        })
        .run();
    });
  }

  // [Order 3-5] SRT 자막 파일 생성
  // [LEARN] SRT 포맷
  // - 가장 널리 사용되는 자막 파일 형식이다.
  // - 형식: 번호 → 시간 범위 (HH:MM:SS,mmm --> HH:MM:SS,mmm) → 자막 텍스트
  /**
   * Generate SRT subtitle file content
   * @param subtitles - Array of subtitles
   * @returns SRT format string
   */
  private generateSRT(subtitles: Subtitle[]): string {
    return subtitles
      .map((sub, index) => {
        const start = this.formatSRTTime(sub.startTime);
        const end = this.formatSRTTime(sub.startTime + sub.duration);

        return `${index + 1}\n${start} --> ${end}\n${sub.text}\n`;
      })
      .join('\n');
  }

  /**
   * Format time in SRT format (HH:MM:SS,mmm)
   * @param seconds - Time in seconds
   * @returns SRT formatted time string
   */
  private formatSRTTime(seconds: number): string {
    const hours = Math.floor(seconds / 3600);
    const minutes = Math.floor((seconds % 3600) / 60);
    const secs = Math.floor(seconds % 60);
    const millis = Math.floor((seconds % 1) * 1000);

    return `${hours.toString().padStart(2, '0')}:${minutes
      .toString()
      .padStart(2, '0')}:${secs.toString().padStart(2, '0')},${millis
      .toString()
      .padStart(3, '0')}`;
  }
}

// [Reader Notes]
// =============================================================================
// 이 파일에서 처음 등장한 개념: fluent-ffmpeg, FFmpeg 필터, 비동기 프로세스 실행
//
// ## FFmpeg 아키텍처 (C 개발자 관점)
// - FFmpeg는 libavcodec, libavformat 등 여러 C 라이브러리로 구성된다.
// - fluent-ffmpeg는 이 CLI 도구를 Node.js에서 편리하게 사용하게 해준다.
// - 내부적으로 child_process.spawn()으로 ffmpeg 프로세스를 실행한다.
// - C에서 fork() + exec()로 외부 명령을 실행하는 것과 동일한 원리이다.
//
// ## Stream Copy vs. 재인코딩
// - Stream Copy (-c copy): 디코딩/인코딩 없이 스트림을 그대로 복사
//   - 장점: 매우 빠름, 무손실
//   - 단점: 키프레임 경계에서만 정확히 자를 수 있음
// - 재인코딩: 완전히 디코드 후 다시 인코드
//   - 장점: 정확한 시간 지정, 필터 적용 가능
//   - 단점: 느림, CPU 집약적, 약간의 품질 손실
//
// ## Phase 2에서의 변화
// - 현재: fluent-ffmpeg로 CLI 호출 (고수준)
// - Phase 2: C++ Native Addon으로 FFmpeg C API 직접 호출 (저수준)
// - 관련 파일: native/src/video_processor.cpp
//
// ## 이 파일을 이해한 다음, 이어서 보면 좋은 파일:
// 1. services/native-video.service.ts - Native C++ 모듈 호출 서비스
// 2. routes/edit.routes.ts - 편집 API가 이 서비스를 사용하는 방법
// =============================================================================
