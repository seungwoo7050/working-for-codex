// [FILE]
// - 목적: 비디오 편집 API 호출을 위한 커스텀 훅
// - 주요 역할: Trim(자르기), Split(분할) 기능의 백엔드 API 호출 및 상태 관리
// - 관련 클론 가이드 단계: [CG-v1.1.0] Trim/Split 기능
// - 권장 읽는 순서: Order 1 (Trim) → Order 2 (Split)
//
// [LEARN] C 개발자를 위한 Fetch API 이해:
// - Fetch는 HTTP 요청을 보내는 브라우저 내장 API이다.
// - C에서 libcurl 같은 HTTP 클라이언트를 사용하는 것과 유사하다.
// - Promise를 반환하므로 async/await와 함께 사용한다.

import { useState } from 'react';
import { TrimParams, SplitParams, EditResult, SplitResult } from '../types/edit';

const API_URL = import.meta.env.VITE_API_URL || 'http://localhost:3001';

/**
 * Hook for video editing operations (trim, split)
 */
export function useVideoEdit() {
  // 공통 상태: 처리 중 여부, 에러 메시지
  const [processing, setProcessing] = useState(false);
  const [error, setError] = useState<string | null>(null);

  // [Order 1] Trim(자르기) 함수
  // - 비디오의 특정 구간만 추출하는 기능
  // - params: { filename, startTime, endTime }
  const trimVideo = async (params: TrimParams): Promise<EditResult | null> => {
    setProcessing(true);
    setError(null);

    try {
      // [LEARN] Fetch API 사용법
      // - fetch(url, options): HTTP 요청을 보내고 Promise<Response>를 반환
      // - method: HTTP 메서드 (GET, POST, PUT, DELETE 등)
      // - headers: 요청 헤더 (Content-Type 등)
      // - body: 요청 바디 (JSON.stringify로 직렬화)
      const response = await fetch(`${API_URL}/api/edit/trim`, {
        method: 'POST',
        headers: {
          'Content-Type': 'application/json',
        },
        body: JSON.stringify(params),
      });

      // [LEARN] HTTP 응답 처리
      // - response.ok: 상태 코드가 200-299인지 확인
      // - response.json(): JSON 바디를 파싱하여 Promise 반환
      if (!response.ok) {
        const errorData = await response.json();
        throw new Error(errorData.error || 'Trim failed');
      }

      const result = await response.json();
      setProcessing(false);
      return result as EditResult;
    } catch (err) {
      // 에러 처리: 상태 업데이트 후 null 반환
      const errorMsg = err instanceof Error ? err.message : 'Trim failed';
      setError(errorMsg);
      setProcessing(false);
      return null;
    }
  };

  // [Order 2] Split(분할) 함수
  // - 비디오를 지정된 시간에서 여러 파트로 분할
  // - params: { filename, splitPoints }
  const splitVideo = async (params: SplitParams): Promise<SplitResult | null> => {
    setProcessing(true);
    setError(null);

    try {
      const response = await fetch(`${API_URL}/api/edit/split`, {
        method: 'POST',
        headers: {
          'Content-Type': 'application/json',
        },
        body: JSON.stringify(params),
      });

      if (!response.ok) {
        const errorData = await response.json();
        throw new Error(errorData.error || 'Split failed');
      }

      const result = await response.json();
      setProcessing(false);
      return result as SplitResult;
    } catch (err) {
      const errorMsg = err instanceof Error ? err.message : 'Split failed';
      setError(errorMsg);
      setProcessing(false);
      return null;
    }
  };

  // [Order 3] 훅 반환값
  // - 두 편집 함수와 공통 상태를 반환
  return {
    trimVideo,
    splitVideo,
    processing,
    error,
  };
}

// [Reader Notes]
// =============================================================================
// 이 파일의 Reader Notes는 useVideoUpload.ts에서 다룬 내용과 중복됩니다.
// Fetch API, async/await, 에러 처리 패턴은 해당 파일의 Reader Notes를 참고하세요.
//
// ## 추가 학습 포인트: RESTful API 설계
// - POST /api/edit/trim: 트림 작업 생성
// - POST /api/edit/split: 분할 작업 생성
// - 백엔드에서 FFmpeg를 호출하여 실제 비디오 처리 수행
//
// ## 이 파일을 이해한 다음, 이어서 보면 좋은 파일:
// 1. backend/src/routes/edit.routes.ts - 편집 API 엔드포인트 정의
// 2. backend/src/services/ffmpeg.service.ts - FFmpeg 호출 서비스
// =============================================================================
