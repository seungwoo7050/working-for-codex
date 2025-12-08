// [FILE]
// - 목적: 비디오 편집 관련 타입 정의
// - 주요 역할: TrimParams, SplitParams, EditResult 인터페이스
// - 관련 클론 가이드 단계: [CG-v1.1.0] Trim/Split
// - 권장 읽는 순서: video.ts 이후
//
// [LEARN] C 개발자를 위한 TypeScript 인터페이스:
// - 인터페이스는 함수 매개변수와 반환값의 "계약"을 정의한다.
// - C의 구조체와 유사하지만 런타임에 존재하지 않는다.
// - API 요청/응답 타입을 명확히 하여 오류를 방지한다.

/**
 * Trim operation parameters
 */
// [LEARN] 트림 요청 파라미터
// - 어떤 파일(filename)의 어느 구간(startTime~endTime)을 자를지 정의
export interface TrimParams {
  filename: string;   // 대상 파일명
  startTime: number;  // 시작 시간 (초)
  endTime: number;    // 종료 시간 (초)
}

/**
 * Split operation parameters
 */
// [LEARN] 분할 요청 파라미터
// - 어떤 파일(filename)을 어느 시점(splitTime)에서 둘로 나눌지 정의
export interface SplitParams {
  filename: string;   // 대상 파일명
  splitTime: number;  // 분할 지점 (초)
}

/**
 * Processing result for edited video
 */
// [LEARN] 편집 결과 타입
// - 서버에서 반환하는 처리 완료 정보
export interface EditResult {
  filename: string;   // 생성된 파일명
  path: string;       // 서버 파일 경로
  url: string;        // 클라이언트 접근 URL
  size: number;       // 파일 크기 (바이트)
  duration?: number;  // 영상 길이 (초, 선택적)
}

/**
 * Split result with two parts
 */
// [LEARN] 분할 결과 타입
// - 분할은 두 개의 파일을 생성하므로 배열로 반환
export interface SplitResult {
  parts: EditResult[];  // 분할된 파트 목록
}

/**
 * Processing state
 */
// [LEARN] 처리 상태 타입
// - UI에서 로딩 표시, 진행률, 에러 표시에 사용
export interface ProcessingState {
  isProcessing: boolean;               // 처리 중 여부
  operation: 'trim' | 'split' | null;  // 현재 작업 종류
  progress: number;                    // 진행률 (0-100)
  error: string | null;                // 에러 메시지
}
