// [FILE]
// - 목적: 대시보드에서 사용하는 Job 타입 정의.
// - 주요 역할: 백엔드와 호환되는 필드 타입을 제공한다.
// - 관련 토이 버전: [FE-F0.2]
// - 권장 읽는 순서: 필드 설명 -> 확장 시 추가 필드 작성 예시.
//
// [LEARN] API 응답 모델을 TypeScript 타입으로 정리하는 기본 패턴을 익힌다.
export type JobStatus = 'PENDING' | 'RUNNING' | 'DONE' | 'FAILED';

export interface Job {
  id: number;
  title: string;
  status: JobStatus;
  createdAt: string;
}
