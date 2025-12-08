// [FILE]
// - 목적: 비디오 파일 업로드 UI 컴포넌트 (드래그 앤 드롭 + 파일 선택)
// - 주요 역할: 사용자로부터 비디오 파일을 받아 서버로 업로드하고 진행률을 표시
// - 관련 클론 가이드 단계: [CG-v1.0.0] 기본 인프라 - 비디오 업로드
// - 권장 읽는 순서: Order 1 (Props/상태) → Order 2 (핸들러) → Order 3 (JSX)
//
// [LEARN] C 개발자를 위한 React 이벤트 처리 이해:
// - C에서 이벤트 루프와 콜백을 직접 구현하듯, React는 이벤트를 자동으로 처리한다.
// - onDrop, onChange 등의 props에 함수를 전달하면, 해당 이벤트 발생 시 호출된다.
// - 브라우저의 DOM 이벤트를 React가 추상화하여 일관된 인터페이스를 제공한다.

import { useCallback, useState } from 'react';
import { useVideoUpload } from '../hooks/useVideoUpload';
import { VideoMetadata } from '../types/video';

// [Order 1-1] 컴포넌트 Props 타입 정의
// - TypeScript interface로 이 컴포넌트가 받을 수 있는 props를 명시한다.
// - C의 함수 시그니처에서 매개변수 타입을 정의하는 것과 동일한 목적이다.
interface VideoUploadProps {
  onVideoUploaded: (video: VideoMetadata) => void;
}

// [Order 1-2] VideoUpload 컴포넌트 정의
/**
 * Video upload component with drag & drop support
 */
export function VideoUpload({ onVideoUploaded }: VideoUploadProps) {
  // [Order 1-3] 커스텀 훅으로 업로드 로직 분리
  // - useVideoUpload 훅은 업로드 상태(uploading, progress, error)와 업로드 함수를 제공한다.
  // - C로 비유하면, 업로드 관련 함수들을 별도 모듈로 분리한 것과 유사하다.
  // - 관련 파일: hooks/useVideoUpload.ts
  const { uploadVideo, uploading, progress, error } = useVideoUpload();
  
  // 드래그 중인지 여부를 추적하는 로컬 상태
  const [isDragging, setIsDragging] = useState(false);

  // [Order 2] 이벤트 핸들러 함수들
  
  // [Order 2-1] 파일 처리 핸들러
  // [LEARN] useCallback 훅이란?
  // - 함수를 "메모이제이션"하여 불필요한 재생성을 방지한다.
  // - C에서 함수 포인터를 캐싱하는 것과 유사한 최적화 기법이다.
  // - 의존성 배열(두 번째 인자)의 값이 변경될 때만 함수를 새로 생성한다.
  const handleFile = useCallback(async (file: File) => {
    // 파일 타입 검증: video/* MIME 타입만 허용
    if (!file.type.startsWith('video/')) {
      alert('Please select a video file');
      return;
    }

    // [LEARN] async/await 패턴
    // - JavaScript의 비동기 처리 문법이다.
    // - C에서 블로킹 I/O 호출 후 결과를 기다리는 것과 결과는 같지만,
    //   내부적으로는 이벤트 루프가 다른 작업을 처리할 수 있게 해준다.
    const result = await uploadVideo(file);
    if (result) {
      // [LEARN] import.meta.env: Vite의 환경 변수 접근 방식
      // - C의 #define이나 환경 변수와 유사하다.
      // - VITE_ 접두사가 붙은 변수만 클라이언트에 노출된다.
      const apiUrl = import.meta.env.VITE_API_URL || 'http://localhost:3001';
      // 부모 컴포넌트에 업로드 완료를 알림 (콜백 호출)
      onVideoUploaded({
        ...result,
        url: `${apiUrl}${result.url}`,
      });
    }
  }, [uploadVideo, onVideoUploaded]);

  // [Order 2-2] 드래그 앤 드롭 이벤트 핸들러들
  // [LEARN] 브라우저 Drag & Drop API
  // - HTML5 표준 API로, 파일을 브라우저 창에 드래그하여 놓을 수 있다.
  // - e.preventDefault()를 호출해야 브라우저의 기본 동작(파일 열기)을 막는다.
  const handleDrop = useCallback((e: React.DragEvent) => {
    e.preventDefault();
    setIsDragging(false);

    const file = e.dataTransfer.files[0];
    if (file) {
      handleFile(file);
    }
  }, [handleFile]);

  const handleDragOver = useCallback((e: React.DragEvent) => {
    e.preventDefault();
    setIsDragging(true);
  }, []);

  const handleDragLeave = useCallback(() => {
    setIsDragging(false);
  }, []);

  // [Order 2-3] 파일 input 변경 핸들러
  // - 일반적인 파일 선택 버튼 클릭 시 호출된다.
  const handleFileInput = useCallback((e: React.ChangeEvent<HTMLInputElement>) => {
    // [LEARN] 옵셔널 체이닝 (?.)
    // - e.target.files가 null/undefined일 수 있으므로 안전하게 접근한다.
    // - C에서 포인터 NULL 체크와 동일한 목적이지만, 문법적으로 간결하다.
    const file = e.target.files?.[0];
    if (file) {
      handleFile(file);
    }
  }, [handleFile]);

  // [Order 3] JSX 렌더링
  return (
    <div className="w-full">
      <div
        className={`border-2 border-dashed rounded-lg p-8 text-center transition-colors ${
          isDragging
            ? 'border-blue-500 bg-blue-500/10'
            : 'border-gray-600 hover:border-gray-500'
        }`}
        onDrop={handleDrop}
        onDragOver={handleDragOver}
        onDragLeave={handleDragLeave}
      >
        {uploading ? (
          <div className="space-y-4">
            <p className="text-gray-300">Uploading... {Math.round(progress)}%</p>
            <div className="w-full bg-gray-700 rounded-full h-2">
              <div
                className="bg-blue-500 h-2 rounded-full transition-all duration-300"
                style={{ width: `${progress}%` }}
              />
            </div>
          </div>
        ) : (
          <div className="space-y-4">
            <div className="text-4xl">📹</div>
            <div>
              <p className="text-gray-300 mb-2">
                Drag & drop your video here, or click to browse
              </p>
              <input
                type="file"
                accept="video/*"
                onChange={handleFileInput}
                className="hidden"
                id="video-input"
              />
              <label
                htmlFor="video-input"
                className="inline-block px-4 py-2 bg-blue-600 hover:bg-blue-700 text-white rounded cursor-pointer transition-colors"
              >
                Select Video
              </label>
            </div>
            <p className="text-sm text-gray-500">
              Supports MP4, MOV, AVI, MKV, WebM (max 500MB)
            </p>
          </div>
        )}
      </div>
      {error && (
        <div className="mt-4 p-3 bg-red-500/10 border border-red-500 rounded text-red-400">
          {error}
        </div>
      )}
    </div>
  );
}

// [Reader Notes]
// =============================================================================
// 이 파일에서 처음 등장한 개념: 커스텀 훅, useCallback, Drag & Drop API
//
// ## 커스텀 훅 패턴 (C 개발자 관점)
// - useVideoUpload()처럼 use로 시작하는 함수는 "커스텀 훅"이다.
// - C에서 관련 함수들을 모듈로 묶듯, React에서는 관련 상태와 로직을 훅으로 묶는다.
// - 재사용 가능하고, 테스트하기 쉽고, 컴포넌트를 깔끔하게 유지할 수 있다.
//
// ## useCallback의 의존성 배열
// - [uploadVideo, onVideoUploaded]는 "이 값들이 변경되면 함수를 새로 만들라"는 의미이다.
// - C에서 함수 포인터가 가리키는 대상이 바뀌면 새 포인터가 필요한 것과 유사하다.
//
// ## 조건부 렌더링 패턴
// - {uploading ? <업로드중UI /> : <대기UI />}: 삼항 연산자로 UI 분기
// - {error && <에러UI />}: 단락 평가로 조건부 렌더링
//
// ## 이 파일을 이해한 다음, 이어서 보면 좋은 파일:
// 1. hooks/useVideoUpload.ts - 실제 업로드 로직과 XHR 사용법
// 2. components/VideoPlayer.tsx - 업로드된 비디오를 재생하는 컴포넌트
// =============================================================================
