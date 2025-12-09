// [FILE]
// - 목적: 프로젝트 CRUD API 호출을 위한 커스텀 훅
// - 주요 역할: 프로젝트 생성, 조회, 수정, 삭제 기능
// - 관련 클론 가이드 단계: [CG-v1.3.0] WebSocket/영속화
// - 권장 읽는 순서: ProjectPanel.tsx와 함께
//
// [LEARN] C 개발자를 위한 CRUD 패턴:
// - CRUD: Create(생성), Read(조회), Update(수정), Delete(삭제)
// - 데이터베이스 조작의 기본 패턴으로, REST API에서도 동일하게 적용
// - HTTP 메서드: POST(생성), GET(조회), PUT(수정), DELETE(삭제)

import { useState } from 'react';

const API_URL = import.meta.env.VITE_API_URL || 'http://localhost:3001';

// [LEARN] 타임라인 상태 인터페이스
// - 프로젝트 저장 시 현재 편집 상태를 함께 저장한다.
// - [key: string]: unknown은 추가 필드를 허용하는 인덱스 시그니처다.
export interface TimelineState {
  currentTime: number;
  duration: number;
  [key: string]: unknown;  // 확장 가능한 추가 속성
}

// [LEARN] 프로젝트 엔티티 인터페이스
// - 데이터베이스 테이블의 행(row)에 대응하는 타입이다.
// - snake_case는 DB 컬럼명과 일치시킨 것이다.
export interface Project {
  id: number;
  name: string;
  description: string | null;
  video_filename: string;
  video_url: string;
  timeline_state: TimelineState;
  created_at: string;
  updated_at: string;
}

// [LEARN] 프로젝트 생성 파라미터
// - API 요청 시 필요한 필드만 정의한다.
// - id, created_at 등은 서버에서 자동 생성된다.
export interface CreateProjectParams {
  name: string;
  description?: string;
  videoFilename: string;
  videoUrl: string;
  timelineState?: TimelineState;
}

/**
 * Hook for project CRUD operations
 */
export function useProjects() {
  const [loading, setLoading] = useState(false);
  const [error, setError] = useState<string | null>(null);

  // [LEARN] Create: 프로젝트 생성
  // - POST 요청으로 새 프로젝트를 생성한다.
  const createProject = async (params: CreateProjectParams): Promise<Project | null> => {
    setLoading(true);
    setError(null);

    try {
      const response = await fetch(`${API_URL}/api/projects`, {
        method: 'POST',
        headers: {
          'Content-Type': 'application/json',
        },
        body: JSON.stringify(params),
      });

      if (!response.ok) {
        const errorData = await response.json();
        throw new Error(errorData.error || 'Failed to create project');
      }

      const project = await response.json();
      setLoading(false);
      return project;
    } catch (err) {
      const errorMsg = err instanceof Error ? err.message : 'Failed to create project';
      setError(errorMsg);
      setLoading(false);
      return null;
    }
  };

  const getProjects = async (): Promise<Project[]> => {
    setLoading(true);
    setError(null);

    try {
      const response = await fetch(`${API_URL}/api/projects`);

      if (!response.ok) {
        throw new Error('Failed to fetch projects');
      }

      const projects = await response.json();
      setLoading(false);
      return projects;
    } catch (err) {
      const errorMsg = err instanceof Error ? err.message : 'Failed to fetch projects';
      setError(errorMsg);
      setLoading(false);
      return [];
    }
  };

  const getProject = async (id: number): Promise<Project | null> => {
    setLoading(true);
    setError(null);

    try {
      const response = await fetch(`${API_URL}/api/projects/${id}`);

      if (!response.ok) {
        throw new Error('Failed to fetch project');
      }

      const project = await response.json();
      setLoading(false);
      return project;
    } catch (err) {
      const errorMsg = err instanceof Error ? err.message : 'Failed to fetch project';
      setError(errorMsg);
      setLoading(false);
      return null;
    }
  };

  const updateProject = async (id: number, updates: Partial<CreateProjectParams>): Promise<Project | null> => {
    setLoading(true);
    setError(null);

    try {
      const response = await fetch(`${API_URL}/api/projects/${id}`, {
        method: 'PUT',
        headers: {
          'Content-Type': 'application/json',
        },
        body: JSON.stringify(updates),
      });

      if (!response.ok) {
        const errorData = await response.json();
        throw new Error(errorData.error || 'Failed to update project');
      }

      const project = await response.json();
      setLoading(false);
      return project;
    } catch (err) {
      const errorMsg = err instanceof Error ? err.message : 'Failed to update project';
      setError(errorMsg);
      setLoading(false);
      return null;
    }
  };

  const deleteProject = async (id: number): Promise<boolean> => {
    setLoading(true);
    setError(null);

    try {
      const response = await fetch(`${API_URL}/api/projects/${id}`, {
        method: 'DELETE',
      });

      if (!response.ok) {
        throw new Error('Failed to delete project');
      }

      setLoading(false);
      return true;
    } catch (err) {
      const errorMsg = err instanceof Error ? err.message : 'Failed to delete project';
      setError(errorMsg);
      setLoading(false);
      return false;
    }
  };

  return {
    createProject,
    getProjects,
    getProject,
    updateProject,
    deleteProject,
    loading,
    error,
  };
}
