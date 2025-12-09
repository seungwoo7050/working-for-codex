import { Job } from './types';

// [FILE]
// - 목적: 백엔드 mini-job-service와 통신하는 API 유틸리티 제공.
// - 주요 역할: Job 목록 조회, 생성 요청을 fetch 기반으로 수행한다.
// - 관련 토이 버전: [FE-F0.3]
// - 권장 읽는 순서: baseUrl -> fetch 래퍼 -> 각 API 함수.
//
// [LEARN] 간단한 fetch 래퍼를 만들어 재사용성을 높이는 방법을 익힌다.
const baseUrl = import.meta.env.VITE_BACKEND_URL || 'http://localhost:8080';

async function request<T>(path: string, options: RequestInit = {}): Promise<T> {
  const response = await fetch(`${baseUrl}${path}`, {
    headers: {
      'Content-Type': 'application/json',
    },
    ...options,
  });
  if (!response.ok) {
    throw new Error(`Request failed: ${response.status}`);
  }
  return (await response.json()) as T;
}

export async function listJobs(): Promise<Job[]> {
  return request<Job[]>('/jobs');
}

export async function createJob(title: string): Promise<Job> {
  return request<Job>('/jobs', {
    method: 'POST',
    body: JSON.stringify({ title }),
  });
}
