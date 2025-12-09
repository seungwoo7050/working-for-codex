// [FILE]
// - 목적: 자막 관련 타입 정의
// - 주요 역할: Subtitle, SubtitleParams, SpeedParams 인터페이스
// - 관련 클론 가이드 단계: [CG-v1.2.0] 자막/속도
// - 권장 읽는 순서: edit.ts 이후
//
// [LEARN] C 개발자를 위한 TypeScript 인터페이스:
// - 자막 데이터 구조와 API 요청 파라미터를 타입으로 정의한다.
// - 컴파일 타임에 타입 오류를 잡아 런타임 오류를 방지한다.

/**
 * Subtitle definition
 */
// [LEARN] 자막 데이터 구조
// - 각 자막은 고유 ID, 텍스트, 시작 시간, 지속 시간을 가진다.
export interface Subtitle {
  id: string;        // 고유 식별자
  text: string;      // 자막 텍스트
  startTime: number; // 시작 시간 (초)
  duration: number;  // 표시 시간 (초)
}

/**
 * Subtitle add/edit params
 */
// [LEARN] 자막 API 요청 파라미터
// - 어떤 비디오에 어떤 자막들을 적용할지 정의
export interface SubtitleParams {
  filename: string;        // 대상 파일명
  subtitles: Subtitle[];   // 적용할 자막 목록
  speed?: number;          // 속도 조절 (선택적, 1.0 = 원본)
}

/**
 * Speed change params
 */
// [LEARN] 속도 변경 API 요청 파라미터
// - 어떤 비디오를 어떤 속도로 변환할지 정의
export interface SpeedParams {
  filename: string;  // 대상 파일명
  speed: number;     // 재생 속도 배율 (0.5 = 절반, 2.0 = 2배속)
}
