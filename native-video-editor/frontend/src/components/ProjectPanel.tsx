// [FILE]
// - 목적: 프로젝트 저장/불러오기 UI 컴포넌트
// - 주요 역할: 프로젝트 목록 표시, 저장, 삭제, 불러오기
// - 관련 클론 가이드 단계: [CG-v1.3.0] WebSocket/영속화
// - 권장 읽는 순서: useProjects.ts 훅과 함께
//
// [LEARN] C 개발자를 위한 CRUD UI 패턴:
// - CRUD (Create, Read, Update, Delete): 데이터 조작의 기본 패턴
// - 각 작업에 대응하는 API 호출 함수를 훅에서 제공받는다.
// - 상태 변경 후 목록을 다시 불러와 UI를 동기화한다.

import { useState, useEffect } from 'react';
import { useProjects, Project } from '../hooks/useProjects';
import { VideoMetadata } from '../types/video';

// [LEARN] Props 인터페이스
// - video: 현재 편집 중인 비디오 (저장 시 필요)
// - onProjectLoad: 프로젝트 불러오기 콜백
interface ProjectPanelProps {
  video: VideoMetadata | null;
  onProjectLoad: (project: Project) => void;
}

/**
 * Project management panel for saving and loading projects
 */
export function ProjectPanel({ video, onProjectLoad }: ProjectPanelProps) {
  // [LEARN] 커스텀 훅으로 API 로직 분리
  // - useProjects()가 API 호출과 로딩 상태를 관리한다.
  // - 컴포넌트는 UI 렌더링에만 집중할 수 있다.
  const { createProject, getProjects, deleteProject, loading } = useProjects();
  const [projects, setProjects] = useState<Project[]>([]);
  const [projectName, setProjectName] = useState('');
  const [showSaveForm, setShowSaveForm] = useState(false);

  // [LEARN] 프로젝트 목록 로드 함수
  const loadProjects = async () => {
    const fetchedProjects = await getProjects();
    setProjects(fetchedProjects);
  };

  // [LEARN] 컴포넌트 마운트 시 프로젝트 목록 로드
  // - 빈 의존성 배열 []로 마운트 시 1회만 실행한다.
  // - eslint-disable는 의도적으로 의존성을 제외할 때 사용한다.
  useEffect(() => {
    loadProjects();
    // eslint-disable-next-line react-hooks/exhaustive-deps
  }, []);

  // [LEARN] 프로젝트 저장 핸들러
  const handleSave = async () => {
    if (!video || !projectName.trim()) {
      alert('Please enter a project name');
      return;
    }

    const project = await createProject({
      name: projectName.trim(),
      videoFilename: video.filename,
      videoUrl: video.url,
      timelineState: {
        currentTime: 0,
        duration: video.duration || 0,
      },
    });

    // [LEARN] 저장 성공 후 UI 정리
    // - 폼 초기화, 저장 폼 닫기, 목록 새로고침
    if (project) {
      setProjectName('');
      setShowSaveForm(false);
      await loadProjects();
    }
  };

  // [LEARN] 프로젝트 삭제 핸들러
  // - confirm()으로 사용자 확인 후 삭제한다.
  const handleDelete = async (id: number) => {
    if (confirm('Are you sure you want to delete this project?')) {
      const success = await deleteProject(id);
      if (success) {
        await loadProjects();
      }
    }
  };

  return (
    <div className="bg-gray-800 rounded-lg p-4">
      <h3 className="text-lg font-semibold mb-4">Projects</h3>

      {/* Save Project */}
      {video && (
        <div className="mb-4">
          {!showSaveForm ? (
            <button
              onClick={() => setShowSaveForm(true)}
              className="w-full px-4 py-2 bg-blue-600 hover:bg-blue-700 rounded transition-colors"
            >
              💾 Save Project
            </button>
          ) : (
            <div className="space-y-2">
              <input
                type="text"
                value={projectName}
                onChange={(e) => setProjectName(e.target.value)}
                placeholder="Project name..."
                className="w-full px-3 py-2 bg-gray-700 border border-gray-600 rounded focus:border-blue-500 focus:outline-none"
              />
              <div className="flex gap-2">
                <button
                  onClick={handleSave}
                  disabled={loading}
                  className="flex-1 px-3 py-2 bg-green-600 hover:bg-green-700 disabled:bg-gray-600 rounded text-sm transition-colors"
                >
                  Save
                </button>
                <button
                  onClick={() => setShowSaveForm(false)}
                  className="flex-1 px-3 py-2 bg-gray-600 hover:bg-gray-500 rounded text-sm transition-colors"
                >
                  Cancel
                </button>
              </div>
            </div>
          )}
        </div>
      )}

      {/* Project List */}
      <div className="space-y-2">
        <div className="text-sm text-gray-400">
          {projects.length} saved project{projects.length !== 1 ? 's' : ''}
        </div>

        {projects.length > 0 ? (
          projects.map((project) => (
            <div
              key={project.id}
              className="p-3 bg-gray-700 rounded space-y-2"
            >
              <div className="font-medium">{project.name}</div>
              {project.description && (
                <div className="text-sm text-gray-400">{project.description}</div>
              )}
              <div className="text-xs text-gray-500">
                {new Date(project.created_at).toLocaleDateString()}
              </div>
              <div className="flex gap-2">
                <button
                  onClick={() => onProjectLoad(project)}
                  className="flex-1 px-2 py-1 bg-blue-600 hover:bg-blue-700 rounded text-sm transition-colors"
                >
                  Load
                </button>
                <button
                  onClick={() => handleDelete(project.id)}
                  className="px-2 py-1 bg-red-600 hover:bg-red-700 rounded text-sm transition-colors"
                >
                  Delete
                </button>
              </div>
            </div>
          ))
        ) : (
          <div className="text-sm text-gray-500 text-center py-4">
            No saved projects yet
          </div>
        )}
      </div>
    </div>
  );
}
