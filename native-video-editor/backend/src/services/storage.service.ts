// [FILE]
// - 목적: 비디오 파일 저장소 관리 서비스
// - 주요 역할: 업로드 디렉터리 관리, 파일 경로 생성, 파일 존재 확인
// - 관련 클론 가이드 단계: [CG-v1.0.0] 기본 인프라
// - 권장 읽는 순서: upload.routes.ts와 함께
//
// [LEARN] C 개발자를 위한 Node.js 파일 시스템 API:
// - fs.promises: Node.js 파일 시스템 모듈의 Promise 버전이다.
// - C의 fopen, fclose, unlink 등과 동일한 기능이지만 비동기로 동작한다.
// - path.join: 운영체제에 맞는 경로 구분자를 자동으로 사용한다.
//
// [Reader Notes]
// - process.cwd(): 현재 작업 디렉터리 (C의 getcwd)
// - path.resolve: 상대 경로를 절대 경로로 변환
// - fs.access: 파일 접근 가능 여부 확인 (C의 access)

import path from 'path';
import { promises as fs } from 'fs';

/**
 * Storage service for managing video file storage
 */
// [LEARN] 단일 책임 원칙
// - 파일 저장소 관련 로직만 담당한다.
// - FFmpegService, 라우트 핸들러 등에서 주입받아 사용한다.
export class StorageService {
  private uploadDir: string;

  // [LEARN] 경로 해석
  // - path.resolve()로 현재 디렉터리 기준 절대 경로를 생성한다.
  // - 상대 경로 혼란을 방지하고 일관된 경로를 보장한다.
  constructor(uploadDir = 'uploads') {
    this.uploadDir = path.resolve(process.cwd(), uploadDir);
  }

  /**
   * Ensure upload directory exists
   */
  // [LEARN] 디렉터리 존재 확인 및 생성
  // - access()로 존재 여부 확인 후, 없으면 mkdir() 호출
  // - recursive: true로 중첩 디렉터리도 생성 (mkdir -p와 동일)
  async ensureUploadDir(): Promise<void> {
    try {
      await fs.access(this.uploadDir);
    } catch {
      await fs.mkdir(this.uploadDir, { recursive: true });
    }
  }

  /**
   * Get upload directory path
   */
  getUploadDir(): string {
    return this.uploadDir;
  }

  /**
   * Get file path for a given filename
   */
  // [LEARN] path.join vs 문자열 연결
  // - path.join()은 OS에 맞는 구분자(/ 또는 \)를 사용한다.
  // - 경로 구성 시 항상 path.join()을 사용해야 한다.
  getFilePath(filename: string): string {
    return path.join(this.uploadDir, filename);
  }

  /**
   * Check if file exists
   */
  // [LEARN] 예외 기반 존재 확인
  // - fs.access()는 파일이 없으면 예외를 던진다.
  // - try-catch로 감싸서 불리언 값으로 변환한다.
  async fileExists(filename: string): Promise<boolean> {
    try {
      await fs.access(this.getFilePath(filename));
      return true;
    } catch {
      return false;
    }
  }

  /**
   * Delete file
   */
  // [LEARN] fs.unlink: 파일 삭제
  // - C의 unlink() 시스템 콜과 동일하다.
  async deleteFile(filename: string): Promise<void> {
    const filePath = this.getFilePath(filename);
    await fs.unlink(filePath);
  }
}
