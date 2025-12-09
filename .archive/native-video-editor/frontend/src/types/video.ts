// [FILE]
// - 목적: 비디오 관련 타입 정의
// - 주요 역할: VideoMetadata, UploadResponse, VideoPlayerState 인터페이스
// - 관련 클론 가이드 단계: [CG-v1.0.0] 기본 인프라
// - 권장 읽는 순서: 프로젝트 진입 시 가장 먼저
//
// [LEARN] C 개발자를 위한 TypeScript 타입 시스템:
// - interface는 객체의 형태(shape)를 정의한다.
// - C의 struct와 유사하지만 컴파일 타임에만 존재한다 (런타임 비용 없음).
// - ?는 선택적 필드를 나타낸다 (값이 없을 수 있음).
//
// [Reader Notes]
// - TypeScript 타입은 .d.ts 파일이나 일반 .ts 파일에 정의할 수 있다.
// - export 키워드로 다른 파일에서 import할 수 있게 한다.
// - 이 파일의 타입들은 프론트엔드 전체에서 사용된다.

/**
 * Video file metadata
 */
// [LEARN] 인터페이스 필드 설명
// - 각 필드는 "이름: 타입" 형태로 정의한다.
// - 모든 필드를 갖춘 객체만 이 타입으로 인정된다.
export interface VideoMetadata {
  url: string;           // 비디오 접근 URL
  path: string;          // 서버 내부 경로
  filename: string;      // 저장된 파일명 (UUID 등)
  originalName: string;  // 원본 파일명
  size: number;          // 파일 크기 (바이트)
  mimetype: string;      // MIME 타입 (예: video/mp4)
  duration?: number;     // 영상 길이 (초) - 선택적
}

/**
 * Video upload response
 */
// [LEARN] API 응답 타입
// - 서버의 업로드 API가 반환하는 데이터 구조를 정의한다.
// - 클라이언트에서 응답을 파싱할 때 타입 안전성을 제공한다.
export interface UploadResponse {
  url: string;
  path: string;
  filename: string;
  originalName: string;
  size: number;
  mimetype: string;
}

/**
 * Video player state
 */
// [LEARN] 상태 타입
// - 비디오 플레이어의 현재 상태를 나타낸다.
// - useState나 useReducer로 관리할 때 이 타입을 사용한다.
export interface VideoPlayerState {
  playing: boolean;    // 재생 중 여부
  currentTime: number; // 현재 위치 (초)
  duration: number;    // 전체 길이 (초)
  volume: number;      // 볼륨 (0-1)
  muted: boolean;      // 음소거 여부
}
