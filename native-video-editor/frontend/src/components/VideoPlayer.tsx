// [FILE]
// - 목적: HTML5 비디오 플레이어 컴포넌트
// - 주요 역할: 비디오 재생/정지, 시간 추적, 볼륨 조절, 외부에서 제어 가능한 인터페이스 제공
// - 관련 클론 가이드 단계: [CG-v1.0.0] 기본 인프라 - 비디오 재생
// - 권장 읽는 순서: Order 1 (타입/Props) → Order 2 (상태/이펙트) → Order 3 (핸들러) → Order 4 (JSX)
//
// [LEARN] C 개발자를 위한 forwardRef 이해:
// - React에서 부모 컴포넌트가 자식의 DOM 요소나 메서드에 직접 접근하려면 forwardRef가 필요하다.
// - C에서 구조체 포인터를 반환하여 호출자가 직접 조작할 수 있게 하는 것과 유사하다.
// - useImperativeHandle과 함께 사용하여 노출할 메서드를 명시적으로 정의한다.

import { useRef, useState, useEffect, useImperativeHandle, forwardRef } from 'react';
import { VideoMetadata, VideoPlayerState } from '../types/video';

// [Order 1] 타입 정의
// - TypeScript로 Props와 외부 공개 인터페이스를 명시한다.

interface VideoPlayerProps {
  video: VideoMetadata;
  onTimeUpdate?: (currentTime: number) => void;
  onDurationChange?: (duration: number) => void;
}

// [Order 1-1] 외부 공개 인터페이스 정의
// - 부모 컴포넌트가 ref를 통해 호출할 수 있는 메서드들이다.
// - C의 헤더 파일에서 공개 API를 선언하는 것과 유사하다.
export interface VideoPlayerRef {
  seekTo: (time: number) => void;
  play: () => void;
  pause: () => void;
}

// [Order 2] VideoPlayer 컴포넌트 정의
// [LEARN] forwardRef란?
// - React 컴포넌트에 ref를 전달받을 수 있게 하는 고차 함수(Higher-Order Function)이다.
// - C에서 함수 포인터를 인자로 받아 래핑하는 패턴과 유사하다.
// - 첫 번째 제네릭은 ref 타입, 두 번째는 props 타입이다.
/**
 * Video player component with playback controls
 */
const VideoPlayerComponent = forwardRef<VideoPlayerRef, VideoPlayerProps>(
  ({ video, onTimeUpdate, onDurationChange }, ref) => {
  // [Order 2-1] 내부 상태 및 참조 선언
  // - videoRef: 실제 <video> DOM 요소에 대한 참조
  // - playerState: 재생 상태를 추적하는 객체 (playing, currentTime, duration, volume, muted)
  const videoRef = useRef<HTMLVideoElement>(null);
  const [playerState, setPlayerState] = useState<VideoPlayerState>({
    playing: false,
    currentTime: 0,
    duration: 0,
    volume: 1,
    muted: false,
  });

  // [Order 2-2] useEffect: 비디오 이벤트 리스너 등록
  // [LEARN] useEffect 훅이란?
  // - 컴포넌트가 렌더링된 후 "부수 효과(side effect)"를 실행하는 훅이다.
  // - C에서 초기화 함수를 main() 시작 부분에서 호출하는 것과 유사하지만,
  //   React는 렌더링 후에 실행되며, 정리(cleanup) 함수도 반환할 수 있다.
  // - 의존성 배열([onTimeUpdate, onDurationChange])의 값이 변경될 때마다 재실행된다.
  useEffect(() => {
    const videoElement = videoRef.current;
    if (!videoElement) return;

    // [LEARN] DOM 이벤트 리스너 패턴
    // - C에서 이벤트 핸들러를 등록하고 해제하는 것과 동일한 패턴이다.
    // - addEventListener로 등록하고, 정리 함수에서 removeEventListener로 해제한다.
    // - 메모리 누수를 방지하기 위해 반드시 정리해야 한다.
    
    const handleTimeUpdate = () => {
      const currentTime = videoElement.currentTime;
      // [LEARN] 함수형 상태 업데이트
      // - setPlayerState(prev => ({ ...prev, currentTime }))는
      //   이전 상태를 기반으로 새 상태를 계산한다.
      // - C에서 구조체를 복사하고 일부 필드만 변경하는 것과 유사하다.
      setPlayerState((prev) => ({ ...prev, currentTime }));
      onTimeUpdate?.(currentTime);
    };

    const handleDurationChange = () => {
      const duration = videoElement.duration;
      setPlayerState((prev) => ({ ...prev, duration }));
      onDurationChange?.(duration);
    };

    const handlePlay = () => {
      setPlayerState((prev) => ({ ...prev, playing: true }));
    };

    const handlePause = () => {
      setPlayerState((prev) => ({ ...prev, playing: false }));
    };

    videoElement.addEventListener('timeupdate', handleTimeUpdate);
    videoElement.addEventListener('durationchange', handleDurationChange);
    videoElement.addEventListener('play', handlePlay);
    videoElement.addEventListener('pause', handlePause);

    return () => {
      videoElement.removeEventListener('timeupdate', handleTimeUpdate);
      videoElement.removeEventListener('durationchange', handleDurationChange);
      videoElement.removeEventListener('play', handlePlay);
      videoElement.removeEventListener('pause', handlePause);
    };
  }, [onTimeUpdate, onDurationChange]);

  // [Order 3] 이벤트 핸들러 함수들
  
  // [Order 3-1] 재생/정지 토글
  const togglePlay = () => {
    const videoElement = videoRef.current;
    if (!videoElement) return;

    if (playerState.playing) {
      videoElement.pause();
    } else {
      videoElement.play();
    }
  };

  // [Order 3-2] 시간 탐색(Seek) 핸들러
  const handleSeek = (e: React.ChangeEvent<HTMLInputElement>) => {
    const videoElement = videoRef.current;
    if (!videoElement) return;

    const time = parseFloat(e.target.value);
    videoElement.currentTime = time;
    setPlayerState((prev) => ({ ...prev, currentTime: time }));
  };

  const handleVolumeChange = (e: React.ChangeEvent<HTMLInputElement>) => {
    const videoElement = videoRef.current;
    if (!videoElement) return;

    const volume = parseFloat(e.target.value);
    videoElement.volume = volume;
    setPlayerState((prev) => ({ ...prev, volume, muted: volume === 0 }));
  };

  // [Order 3-4] 음소거 토글
  const toggleMute = () => {
    const videoElement = videoRef.current;
    if (!videoElement) return;

    const newMuted = !playerState.muted;
    videoElement.muted = newMuted;
    setPlayerState((prev) => ({ ...prev, muted: newMuted }));
  };

  // [Order 3-5] 시간 포맷팅 유틸리티 함수
  // - 초 단위 시간을 "MM:SS" 형식으로 변환한다.
  const formatTime = (seconds: number): string => {
    const mins = Math.floor(seconds / 60);
    const secs = Math.floor(seconds % 60);
    return `${mins}:${secs.toString().padStart(2, '0')}`;
  };

  // [Order 3-6] useImperativeHandle: 부모에게 공개할 메서드 정의
  // [LEARN] useImperativeHandle이란?
  // - forwardRef로 전달받은 ref에 어떤 값/메서드를 노출할지 정의한다.
  // - C에서 구조체의 public 멤버만 헤더에 노출하는 것과 유사하다.
  // - 캡슐화: 내부 구현(videoRef)은 숨기고, 필요한 API만 노출한다.
  useImperativeHandle(ref, () => ({
    seekTo: (time: number) => {
      if (videoRef.current) {
        videoRef.current.currentTime = time;
      }
    },
    play: () => {
      videoRef.current?.play();
    },
    pause: () => {
      videoRef.current?.pause();
    },
  }));

  // [Order 4] JSX 렌더링
  return (
    <div className="space-y-4">
      <div className="bg-black rounded-lg overflow-hidden">
        <video
          ref={videoRef}
          src={video.url}
          className="w-full"
          onError={() => {
            // Video load error - handled by UI
          }}
        />
      </div>

      <div className="space-y-3 bg-gray-800 rounded-lg p-4">
        {/* Seek bar */}
        <div className="space-y-1">
          <input
            type="range"
            min="0"
            max={playerState.duration || 0}
            value={playerState.currentTime}
            onChange={handleSeek}
            className="w-full h-2 bg-gray-700 rounded-lg appearance-none cursor-pointer"
            style={{
              background: `linear-gradient(to right, #3b82f6 0%, #3b82f6 ${
                (playerState.currentTime / (playerState.duration || 1)) * 100
              }%, #374151 ${
                (playerState.currentTime / (playerState.duration || 1)) * 100
              }%, #374151 100%)`,
            }}
          />
          <div className="flex justify-between text-xs text-gray-400">
            <span>{formatTime(playerState.currentTime)}</span>
            <span>{formatTime(playerState.duration)}</span>
          </div>
        </div>

        {/* Controls */}
        <div className="flex items-center gap-4">
          <button
            onClick={togglePlay}
            className="px-4 py-2 bg-blue-600 hover:bg-blue-700 rounded transition-colors"
          >
            {playerState.playing ? '⏸ Pause' : '▶ Play'}
          </button>

          <div className="flex items-center gap-2">
            <button onClick={toggleMute} className="hover:text-blue-400 transition-colors">
              {playerState.muted ? '🔇' : '🔊'}
            </button>
            <input
              type="range"
              min="0"
              max="1"
              step="0.1"
              value={playerState.volume}
              onChange={handleVolumeChange}
              className="w-20 h-2 bg-gray-700 rounded-lg appearance-none cursor-pointer"
            />
          </div>

          <div className="text-sm text-gray-400">
            {video.originalName}
          </div>
        </div>
      </div>
    </div>
  );
});

// [LEARN] displayName 설정
// - React DevTools에서 컴포넌트 이름을 표시하기 위해 필요하다.
// - forwardRef로 감싼 컴포넌트는 이름이 자동 추론되지 않으므로 명시적으로 설정한다.
VideoPlayerComponent.displayName = 'VideoPlayer';

export const VideoPlayer = VideoPlayerComponent;

// [Reader Notes]
// =============================================================================
// 이 파일에서 처음 등장한 개념: forwardRef, useImperativeHandle, useEffect 정리 함수
//
// ## forwardRef + useImperativeHandle 패턴 (C 개발자 관점)
// - 일반적인 React 데이터 흐름: 부모 → 자식 (Props, 단방향)
// - 때때로 부모가 자식을 직접 제어해야 할 때: forwardRef + useImperativeHandle
// - C 비유:
//   - 일반 패턴: 함수 호출 시 인자로 데이터 전달
//   - forwardRef 패턴: 구조체 포인터를 반환하여 호출자가 직접 멤버 접근
//
// ## useEffect 정리(Cleanup) 함수
// - useEffect가 반환하는 함수는 컴포넌트가 언마운트되거나 의존성이 변경될 때 호출된다.
// - C에서 malloc/free 쌍처럼, addEventListener/removeEventListener 쌍을 맞춰야 한다.
// - 메모리 누수와 좀비 이벤트 핸들러를 방지한다.
//
// ## HTML5 Video API 요약
// - videoElement.play(), pause(): 재생 제어
// - videoElement.currentTime: 현재 재생 위치 (초)
// - videoElement.duration: 전체 길이 (초)
// - 이벤트: timeupdate, durationchange, play, pause
//
// ## 이 파일을 이해한 다음, 이어서 보면 좋은 파일:
// 1. components/Timeline.tsx - Canvas를 이용한 타임라인 UI
// 2. hooks/useVideoEdit.ts - 비디오 편집 API 호출 훅
// =============================================================================
