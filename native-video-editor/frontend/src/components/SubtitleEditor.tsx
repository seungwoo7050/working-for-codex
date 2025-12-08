// [FILE]
// - 목적: 자막 편집 UI 컴포넌트
// - 주요 역할: 자막 추가/수정/삭제, 시간 조절
// - 관련 클론 가이드 단계: [CG-v1.2.0] 자막/속도
// - 권장 읽는 순서: EditPanel.tsx 이후
//
// [LEARN] C 개발자를 위한 리스트 렌더링:
// - 배열.map()으로 각 요소를 JSX로 변환한다.
// - key 속성은 React가 리스트 항목을 효율적으로 업데이트하기 위한 식별자다.
// - C에서 배열을 순회하며 출력하는 것과 유사하지만, 반환값이 UI 요소다.

import { useState } from 'react';
import { Subtitle } from '../types/subtitle';

// [LEARN] Props 인터페이스
// - 부모(EditPanel)로부터 현재 자막 목록과 변경 콜백을 받는다.
interface SubtitleEditorProps {
  subtitles: Subtitle[];                            // 현재 자막 목록
  currentTime: number;                              // 현재 재생 시간 (초)
  onSubtitlesChange: (subtitles: Subtitle[]) => void;  // 자막 변경 콜백
}

/**
 * Subtitle editor component for adding, editing, and removing subtitles
 */
export function SubtitleEditor({ subtitles, currentTime, onSubtitlesChange }: SubtitleEditorProps) {
  // [LEARN] 폼 입력 상태 관리
  // - 각 입력 필드마다 useState로 상태를 관리한다.
  // - editingId: 현재 수정 중인 자막의 ID (수정 모드 전환용)
  const [editingId, setEditingId] = useState<string | null>(null);
  const [newSubText, setNewSubText] = useState('');
  const [newSubStart, setNewSubStart] = useState(0);
  const [newSubDuration, setNewSubDuration] = useState(3);

  // [LEARN] 시간 포맷팅 유틸리티
  // - 초 단위를 "분:초" 형식으로 변환한다.
  const formatTime = (seconds: number): string => {
    const mins = Math.floor(seconds);
    const secs = Math.floor((seconds % 1) * 60);
    return `${mins}:${secs.toString().padStart(2, '0')}`;
  };

  // [LEARN] 자막 추가 핸들러
  // - 입력 검증 후 새 자막을 생성하여 목록에 추가한다.
  // - sort()로 시작 시간 순서를 유지한다.
  const handleAdd = () => {
    if (!newSubText.trim()) {
      alert('Please enter subtitle text');
      return;
    }

    const newSubtitle: Subtitle = {
      id: `sub-${Date.now()}`,  // 타임스탬프 기반 고유 ID
      text: newSubText.trim(),
      startTime: newSubStart,
      duration: newSubDuration,
    };

    // [LEARN] 불변성 패턴
    // - ...subtitles로 기존 배열을 복사하고 새 항목을 추가한다.
    // - React는 배열 참조가 바뀌어야 리렌더링한다.
    onSubtitlesChange([...subtitles, newSubtitle].sort((a, b) => a.startTime - b.startTime));
    setNewSubText('');
    setNewSubStart(currentTime);
  };

  // [LEARN] 자막 삭제 핸들러
  // - filter()로 해당 ID를 제외한 새 배열을 생성한다.
  const handleRemove = (id: string) => {
    onSubtitlesChange(subtitles.filter((sub) => sub.id !== id));
  };

  // [LEARN] 자막 수정 핸들러
  // - map()으로 특정 ID의 자막만 업데이트한다.
  // - Partial<Subtitle>: 일부 필드만 업데이트 가능
  const handleEdit = (id: string, updates: Partial<Subtitle>) => {
    onSubtitlesChange(
      subtitles.map((sub) =>
        sub.id === id ? { ...sub, ...updates } : sub
      )
    );
    setEditingId(null);
  };

  const setStartToCurrent = () => {
    setNewSubStart(currentTime);
  };

  return (
    <div className="space-y-4">
      <h4 className="font-medium text-gray-300">Subtitles</h4>

      {/* Add New Subtitle */}
      <div className="space-y-3 p-3 bg-gray-700 rounded">
        <div>
          <label className="block text-sm text-gray-400 mb-1">Text</label>
          <input
            type="text"
            value={newSubText}
            onChange={(e) => setNewSubText(e.target.value)}
            placeholder="Enter subtitle text..."
            className="w-full px-3 py-2 bg-gray-800 border border-gray-600 rounded focus:border-blue-500 focus:outline-none"
          />
        </div>

        <div className="grid grid-cols-2 gap-2">
          <div>
            <label className="block text-sm text-gray-400 mb-1">
              Start: {formatTime(newSubStart)}
            </label>
            <div className="flex gap-1">
              <input
                type="number"
                value={newSubStart.toFixed(1)}
                onChange={(e) => setNewSubStart(parseFloat(e.target.value) || 0)}
                step="0.1"
                className="flex-1 px-2 py-1 bg-gray-800 border border-gray-600 rounded text-sm focus:border-blue-500 focus:outline-none"
              />
              <button
                onClick={setStartToCurrent}
                className="px-2 py-1 bg-gray-600 hover:bg-gray-500 rounded text-xs transition-colors"
              >
                Now
              </button>
            </div>
          </div>

          <div>
            <label className="block text-sm text-gray-400 mb-1">
              Duration: {newSubDuration}s
            </label>
            <input
              type="number"
              value={newSubDuration}
              onChange={(e) => setNewSubDuration(parseFloat(e.target.value) || 1)}
              step="0.5"
              min="0.5"
              max="10"
              className="w-full px-2 py-1 bg-gray-800 border border-gray-600 rounded text-sm focus:border-blue-500 focus:outline-none"
            />
          </div>
        </div>

        <button
          onClick={handleAdd}
          className="w-full px-3 py-2 bg-blue-600 hover:bg-blue-700 rounded text-sm font-medium transition-colors"
        >
          + Add Subtitle
        </button>
      </div>

      {/* Subtitle List */}
      {subtitles.length > 0 && (
        <div className="space-y-2">
          <div className="text-sm text-gray-400">
            {subtitles.length} subtitle{subtitles.length !== 1 ? 's' : ''}
          </div>
          {subtitles.map((sub) => (
            <div
              key={sub.id}
              className="p-3 bg-gray-700 rounded space-y-2"
            >
              {editingId === sub.id ? (
                <>
                  <input
                    type="text"
                    value={sub.text}
                    onChange={(e) => handleEdit(sub.id, { text: e.target.value })}
                    className="w-full px-2 py-1 bg-gray-800 border border-gray-600 rounded text-sm focus:border-blue-500 focus:outline-none"
                  />
                  <div className="flex gap-2">
                    <button
                      onClick={() => setEditingId(null)}
                      className="px-2 py-1 bg-gray-600 hover:bg-gray-500 rounded text-xs transition-colors"
                    >
                      Done
                    </button>
                  </div>
                </>
              ) : (
                <>
                  <div className="text-sm">{sub.text}</div>
                  <div className="flex items-center justify-between text-xs text-gray-400">
                    <span>
                      {formatTime(sub.startTime)} - {formatTime(sub.startTime + sub.duration)}
                      {' '}({sub.duration}s)
                    </span>
                    <div className="flex gap-1">
                      <button
                        onClick={() => setEditingId(sub.id)}
                        className="px-2 py-1 bg-gray-600 hover:bg-gray-500 rounded transition-colors"
                      >
                        Edit
                      </button>
                      <button
                        onClick={() => handleRemove(sub.id)}
                        className="px-2 py-1 bg-red-600 hover:bg-red-700 rounded transition-colors"
                      >
                        Remove
                      </button>
                    </div>
                  </div>
                </>
              )}
            </div>
          ))}
        </div>
      )}
    </div>
  );
}
