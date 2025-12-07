import { useRef, useState, useEffect, useImperativeHandle, forwardRef } from 'react';
import { VideoMetadata, VideoPlayerState } from '../types/video';

interface VideoPlayerProps {
  video: VideoMetadata;
  onTimeUpdate?: (currentTime: number) => void;
  onDurationChange?: (duration: number) => void;
}

export interface VideoPlayerRef {
  seekTo: (time: number) => void;
  play: () => void;
  pause: () => void;
}

/**
 * Video player component with playback controls
 */
export const VideoPlayer = forwardRef<VideoPlayerRef, VideoPlayerProps>(
  ({ video, onTimeUpdate, onDurationChange }, ref) => {
  const videoRef = useRef<HTMLVideoElement>(null);
  const [playerState, setPlayerState] = useState<VideoPlayerState>({
    playing: false,
    currentTime: 0,
    duration: 0,
    volume: 1,
    muted: false,
  });

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

  useEffect(() => {
    const videoElement = videoRef.current;
    if (!videoElement) return;

    const handleTimeUpdate = () => {
      const currentTime = videoElement.currentTime;
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

  const togglePlay = () => {
    const videoElement = videoRef.current;
    if (!videoElement) return;

    if (playerState.playing) {
      videoElement.pause();
    } else {
      videoElement.play();
    }
  };

  const formatTime = (seconds: number): string => {
    const mins = Math.floor(seconds / 60);
    const secs = Math.floor(seconds % 60);
    return `${mins}:${secs.toString().padStart(2, '0')}`;
  };

  return (
    <div className="bg-gray-800 rounded-lg overflow-hidden">
      <video
        ref={videoRef}
        src={video.url}
        className="w-full aspect-video bg-black"
        onClick={togglePlay}
      />
      <div className="p-4 space-y-3">
        <div className="flex items-center justify-between">
          <button
            onClick={togglePlay}
            className="px-4 py-2 bg-blue-600 hover:bg-blue-700 rounded transition-colors"
          >
            {playerState.playing ? '⏸ Pause' : '▶ Play'}
          </button>
          <span className="text-gray-400">
            {formatTime(playerState.currentTime)} / {formatTime(playerState.duration)}
          </span>
        </div>
        <input
          type="range"
          min={0}
          max={playerState.duration || 100}
          value={playerState.currentTime}
          onChange={(e) => {
            if (videoRef.current) {
              videoRef.current.currentTime = parseFloat(e.target.value);
            }
          }}
          className="w-full h-2 bg-gray-700 rounded-lg appearance-none cursor-pointer"
        />
      </div>
    </div>
  );
});

VideoPlayer.displayName = 'VideoPlayer';
