// [FILE]
// - 목적: 비디오 편집 컨트롤 패널 (트림, 분할)
// - 주요 역할: 편집 모드 선택, 시간 범위 설정, 편집 실행
// - 관련 클론 가이드 단계: [CG-v1.1.0] Trim/Split
// - 권장 읽는 순서: EditPanel.tsx 이후 참고용
//
// [LEARN] C 개발자를 위한 React 컴포넌트 구조:
// - 이 컴포넌트는 EditPanel의 단순화된 버전이다.
// - 트림과 분할 기능만 제공한다.

import { useState, useEffect } from 'react';
import { VideoMetadata } from '../types/video';
import { useVideoEdit } from '../hooks/useVideoEdit';
import { EditResult } from '../types/edit';

interface ControlPanelProps {
  video: VideoMetadata;
  duration: number;
  currentTime: number;
  onTrimComplete?: (result: EditResult) => void;
  onSplitComplete?: (results: EditResult[]) => void;
}

/**
 * Control panel for video editing operations
 */
export function ControlPanel({
  video,
  duration,
  currentTime,
  onTrimComplete,
  onSplitComplete,
}: ControlPanelProps) {
  const { trimVideo, splitVideo, processing, error } = useVideoEdit();

  // [LEARN] 유니온 타입 리터럴
  // - mode는 'trim' 또는 'split' 문자열만 가질 수 있다.
  // - C의 enum과 유사하지만 문자열 값을 사용한다.
  const [trimStart, setTrimStart] = useState(0);
  const [trimEnd, setTrimEnd] = useState(duration);
  const [splitTime, setSplitTime] = useState(duration / 2);
  const [mode, setMode] = useState<'trim' | 'split'>('trim');

  // Update values when duration changes
  useEffect(() => {
    setTrimEnd(duration);
    setSplitTime(duration / 2);
  }, [duration]);

  const formatTime = (seconds: number): string => {
    const mins = Math.floor(seconds);
    const secs = Math.floor((seconds % 1) * 60);
    return `${mins}:${secs.toString().padStart(2, '0')}`;
  };

  const handleTrim = async () => {
    if (trimStart >= trimEnd) {
      alert('End time must be greater than start time');
      return;
    }

    const result = await trimVideo({
      filename: video.filename,
      startTime: trimStart,
      endTime: trimEnd,
    });

    if (result && onTrimComplete) {
      onTrimComplete(result);
    }
  };

  const handleSplit = async () => {
    if (splitTime <= 0 || splitTime >= duration) {
      alert('Split time must be between 0 and video duration');
      return;
    }

    const result = await splitVideo({
      filename: video.filename,
      splitTime,
    });

    if (result && onSplitComplete) {
      onSplitComplete(result.parts);
    }
  };

  const setTrimStartToCurrent = () => {
    setTrimStart(currentTime);
  };

  const setTrimEndToCurrent = () => {
    setTrimEnd(currentTime);
  };

  const setSplitTimeToCurrent = () => {
    setSplitTime(currentTime);
  };

  return (
    <div className="bg-gray-800 rounded-lg p-6 space-y-4">
      <h3 className="text-lg font-semibold mb-4">Video Editing</h3>

      {/* Mode Selection */}
      <div className="flex gap-2 mb-4">
        <button
          onClick={() => setMode('trim')}
          className={`px-4 py-2 rounded transition-colors ${
            mode === 'trim'
              ? 'bg-blue-600 text-white'
              : 'bg-gray-700 text-gray-300 hover:bg-gray-600'
          }`}
        >
          ✂️ Trim
        </button>
        <button
          onClick={() => setMode('split')}
          className={`px-4 py-2 rounded transition-colors ${
            mode === 'split'
              ? 'bg-blue-600 text-white'
              : 'bg-gray-700 text-gray-300 hover:bg-gray-600'
          }`}
        >
          ✄ Split
        </button>
      </div>

      {/* Trim Controls */}
      {mode === 'trim' && (
        <div className="space-y-4">
          <div>
            <label className="block text-sm font-medium text-gray-300 mb-2">
              Start Time: {formatTime(trimStart)}
            </label>
            <div className="flex gap-2">
              <input
                type="range"
                min="0"
                max={duration || 100}
                step="0.1"
                value={trimStart}
                onChange={(e) => setTrimStart(parseFloat(e.target.value))}
                className="flex-1 h-2 bg-gray-700 rounded-lg appearance-none cursor-pointer"
              />
              <button
                onClick={setTrimStartToCurrent}
                className="px-3 py-1 bg-gray-700 hover:bg-gray-600 rounded text-sm transition-colors"
              >
                Current
              </button>
            </div>
          </div>

          <div>
            <label className="block text-sm font-medium text-gray-300 mb-2">
              End Time: {formatTime(trimEnd)}
            </label>
            <div className="flex gap-2">
              <input
                type="range"
                min="0"
                max={duration || 100}
                step="0.1"
                value={trimEnd}
                onChange={(e) => setTrimEnd(parseFloat(e.target.value))}
                className="flex-1 h-2 bg-gray-700 rounded-lg appearance-none cursor-pointer"
              />
              <button
                onClick={setTrimEndToCurrent}
                className="px-3 py-1 bg-gray-700 hover:bg-gray-600 rounded text-sm transition-colors"
              >
                Current
              </button>
            </div>
          </div>

          <div className="text-sm text-gray-400">
            Output duration: {formatTime(trimEnd - trimStart)}
          </div>

          <button
            onClick={handleTrim}
            disabled={processing || trimStart >= trimEnd}
            className="w-full px-4 py-2 bg-blue-600 hover:bg-blue-700 disabled:bg-gray-600 disabled:cursor-not-allowed rounded transition-colors font-medium"
          >
            {processing ? 'Processing...' : 'Trim Video'}
          </button>
        </div>
      )}

      {/* Split Controls */}
      {mode === 'split' && (
        <div className="space-y-4">
          <div>
            <label className="block text-sm font-medium text-gray-300 mb-2">
              Split At: {formatTime(splitTime)}
            </label>
            <div className="flex gap-2">
              <input
                type="range"
                min="0"
                max={duration || 100}
                step="0.1"
                value={splitTime}
                onChange={(e) => setSplitTime(parseFloat(e.target.value))}
                className="flex-1 h-2 bg-gray-700 rounded-lg appearance-none cursor-pointer"
              />
              <button
                onClick={setSplitTimeToCurrent}
                className="px-3 py-1 bg-gray-700 hover:bg-gray-600 rounded text-sm transition-colors"
              >
                Current
              </button>
            </div>
          </div>

          <div className="text-sm text-gray-400 space-y-1">
            <div>Part 1: 0:00 - {formatTime(splitTime)} ({formatTime(splitTime)})</div>
            <div>Part 2: {formatTime(splitTime)} - {formatTime(duration)} ({formatTime(duration - splitTime)})</div>
          </div>

          <button
            onClick={handleSplit}
            disabled={processing || splitTime <= 0 || splitTime >= duration}
            className="w-full px-4 py-2 bg-blue-600 hover:bg-blue-700 disabled:bg-gray-600 disabled:cursor-not-allowed rounded transition-colors font-medium"
          >
            {processing ? 'Processing...' : 'Split Video'}
          </button>
        </div>
      )}

      {/* Error Display */}
      {error && (
        <div className="p-3 bg-red-500/10 border border-red-500 rounded text-red-400 text-sm">
          {error}
        </div>
      )}

      {/* Processing Indicator */}
      {processing && (
        <div className="p-3 bg-blue-500/10 border border-blue-500 rounded text-blue-400 text-sm">
          Processing video... This may take a few seconds.
        </div>
      )}
    </div>
  );
}
