// [FILE]
// - 목적: 비디오 업로드 로직을 캡슐화한 커스텀 훅
// - 주요 역할: FormData 생성, XHR 업로드, 진행률 추적, 에러 처리
// - 관련 클론 가이드 단계: [CG-v1.0.0] 기본 인프라 - 비디오 업로드
// - 권장 읽는 순서: Order 1 (상태) → Order 2 (업로드 함수) → Order 3 (반환값)
//
// [LEARN] C 개발자를 위한 커스텀 훅 이해:
// - 커스텀 훅은 "상태 + 로직"을 재사용 가능한 단위로 묶는 패턴이다.
// - C에서 관련 함수들과 전역 상태를 하나의 .c/.h 파일로 모듈화하는 것과 유사하다.
// - 훅 이름은 반드시 'use'로 시작해야 React가 훅 규칙을 적용한다.

import { useState } from 'react';
import { UploadResponse } from '../types/video';

// 환경 변수에서 API URL을 가져오거나 기본값 사용
const API_URL = import.meta.env.VITE_API_URL || 'http://localhost:3001';

// [Order 1] 커스텀 훅 정의
/**
 * Hook for uploading videos to the server
 */
export function useVideoUpload() {
  // [Order 1-1] 훅 내부 상태
  // - uploading: 업로드 진행 중 여부
  // - progress: 업로드 진행률 (0-100)
  // - error: 에러 메시지 (있는 경우)
  const [uploading, setUploading] = useState(false);
  const [progress, setProgress] = useState(0);
  const [error, setError] = useState<string | null>(null);

  // [Order 2] 업로드 함수 정의
  // [LEARN] async 함수와 Promise
  // - async 함수는 항상 Promise를 반환한다.
  // - C에서 비동기 작업 후 콜백을 호출하는 패턴과 유사하지만,
  //   문법적으로 동기 코드처럼 작성할 수 있다.
  const uploadVideo = async (file: File): Promise<UploadResponse | null> => {
    // 업로드 시작 시 상태 초기화
    setUploading(true);
    setProgress(0);
    setError(null);

    try {
      // [LEARN] FormData API
      // - HTML 폼 데이터를 JavaScript에서 생성할 수 있는 Web API이다.
      // - multipart/form-data 형식으로 파일을 전송할 때 사용한다.
      // - C에서 HTTP 요청 바디를 직접 구성하는 것을 추상화한 것이다.
      const formData = new FormData();
      formData.append('video', file);

      // [LEARN] XMLHttpRequest (XHR) vs Fetch API
      // - XHR은 오래된 API지만 업로드 진행률(progress) 이벤트를 지원한다.
      // - Fetch API는 더 현대적이지만 업로드 진행률 추적이 제한적이다.
      // - 업로드 진행률이 필요한 경우 XHR을 사용한다.
      const xhr = new XMLHttpRequest();

      // [LEARN] Promise 생성자 패턴
      // - 콜백 기반 API(XHR)를 Promise로 래핑하는 패턴이다.
      // - resolve: 성공 시 호출, reject: 실패 시 호출
      // - C에서 비동기 작업의 완료를 시그널링하는 것과 유사하다.
      return new Promise((resolve, reject) => {
        // [Order 2-1] 업로드 진행률 이벤트 핸들러
        // - xhr.upload 객체의 progress 이벤트로 업로드 진행률을 추적한다.
        // - e.lengthComputable: 전체 크기를 알 수 있는지 여부
        xhr.upload.addEventListener('progress', (e) => {
          if (e.lengthComputable) {
            const percentComplete = (e.loaded / e.total) * 100;
            setProgress(percentComplete);
          }
        });

        // [Order 2-2] 업로드 완료 이벤트 핸들러
        xhr.addEventListener('load', () => {
          setUploading(false);
          if (xhr.status === 200) {
            // 성공: JSON 응답 파싱 후 resolve
            const response = JSON.parse(xhr.responseText);
            resolve(response);
          } else {
            // HTTP 에러: 에러 상태 설정 후 reject
            const errorMsg = 'Upload failed';
            setError(errorMsg);
            reject(new Error(errorMsg));
          }
        });

        // [Order 2-3] 네트워크 에러 핸들러
        xhr.addEventListener('error', () => {
          const errorMsg = 'Upload failed';
          setUploading(false);
          setError(errorMsg);
          reject(new Error(errorMsg));
        });

        // [Order 2-4] HTTP 요청 전송
        // - POST 메서드로 /api/upload 엔드포인트에 요청
        // - FormData를 바디로 전송하면 Content-Type이 자동으로 multipart/form-data로 설정됨
        xhr.open('POST', `${API_URL}/api/upload`);
        xhr.send(formData);
      });
    } catch (err) {
      // 예외 처리: 상태 업데이트 후 null 반환
      const errorMsg = err instanceof Error ? err.message : 'Upload failed';
      setError(errorMsg);
      setUploading(false);
      return null;
    }
  };

  // [Order 3] 훅 반환값
  // - 훅을 사용하는 컴포넌트에게 상태와 함수를 객체로 반환한다.
  // - 구조 분해 할당으로 필요한 것만 가져갈 수 있다.
  // - 예: const { uploadVideo, uploading, progress } = useVideoUpload();
  return {
    uploadVideo,
    uploading,
    progress,
    error,
  };
}

// [Reader Notes]
// =============================================================================
// 이 파일에서 처음 등장한 개념: XMLHttpRequest, FormData, Promise 래핑
//
// ## 커스텀 훅의 장점 (C 개발자 관점)
// 1. **재사용성**: 여러 컴포넌트에서 동일한 업로드 로직을 사용 가능
// 2. **캡슐화**: 업로드 관련 상태와 로직이 한 곳에 모여 있음
// 3. **테스트 용이성**: 훅만 따로 테스트 가능
// 4. **관심사 분리**: UI 컴포넌트는 렌더링에만 집중
//
// ## XHR vs Fetch API 비교
// | 기능 | XHR | Fetch |
// |------|-----|-------|
// | 업로드 진행률 | ✅ 지원 | ❌ 미지원 |
// | 문법 | 콜백 기반 | Promise 기반 |
// | 취소 | abort() | AbortController |
//
// ## Promise 생성자 패턴
// - 콜백 API → Promise 변환에 사용
// - new Promise((resolve, reject) => { ... })
// - resolve(값): 성공 시 .then()으로 전달
// - reject(에러): 실패 시 .catch()로 전달
//
// ## 이 파일을 이해한 다음, 이어서 보면 좋은 파일:
// 1. hooks/useWebSocket.ts - WebSocket 기반 실시간 통신 훅
// 2. hooks/useVideoEdit.ts - 비디오 편집 API 호출 훅
// =============================================================================
