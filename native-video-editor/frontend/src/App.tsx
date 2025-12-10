// [FILE]
// - 목적: 비디오 에디터 애플리케이션의 최상위 컴포넌트 (Root Component)
// - 주요 역할: 전체 앱 상태 관리, 레이아웃 구성, 자식 컴포넌트 조합
// - 관련 클론 가이드 단계: [CG-v1.0.0] 기본 인프라, [CG-v1.3.0] WebSocket 연동
// - 권장 읽는 순서: Order 1 (상태 정의) → Order 2 (이벤트 핸들러) → Order 3 (JSX 렌더링)
//
// [LEARN] C 개발자를 위한 React 컴포넌트 이해:
// - React 컴포넌트 = C의 "함수 + 구조체"의 조합과 유사하다.
// - 컴포넌트 함수는 "현재 상태(state)"를 입력받아 "UI(JSX)"를 반환한다.
// - C에서 구조체로 상태를 관리하고 함수로 처리하듯, React에서는 useState 훅으로 상태를 관리한다.

import { useState, useRef } from 'react';
import { VideoUpload } from './components/VideoUpload';
import { VideoPlayer, VideoPlayerRef } from './components/VideoPlayer';
import { Timeline } from './components/Timeline';
import { EditPanel } from './components/EditPanel';
import { ProjectPanel } from './components/ProjectPanel';
import { ProgressBar } from './components/ProgressBar';
import { VideoMetadata } from './types/video';
import { EditResult } from './types/edit';
import { useWebSocket } from './hooks/useWebSocket';
import { Project } from './hooks/useProjects';

// [Order 1] 메인 App 컴포넌트 정의
// - 이 함수가 호출될 때마다 UI가 "재계산"된다 (리렌더링).
// - C의 main()과 달리, React 컴포넌트는 상태가 변경될 때마다 반복 호출된다.
function App() {
  // [Order 1-1] 상태(State) 선언부
  // [LEARN] useState 훅이란?
  // - React의 "상태 관리" 메커니즘이다.
  // - C에서 전역/정적 변수로 상태를 유지하듯, useState는 렌더링 간에 값을 유지한다.
  // - const [값, 설정함수] = useState(초기값) 형태로 사용한다.
  // - 설정함수(setter)를 호출하면 React가 컴포넌트를 다시 렌더링한다.
  const [video, setVideo] = useState<VideoMetadata | null>(null);
  const [currentTime, setCurrentTime] = useState(0);
  const [duration, setDuration] = useState(0);
  const [processedVideos, setProcessedVideos] = useState<EditResult[]>([]);
  
  // [LEARN] useRef 훅이란?
  // - DOM 요소나 컴포넌트 인스턴스에 대한 "참조"를 유지한다.
  // - C에서 포인터로 특정 메모리 주소를 가리키듯, ref는 특정 DOM/컴포넌트를 가리킨다.
  // - useState와 달리, ref 값이 변경되어도 리렌더링이 발생하지 않는다.
  const playerRef = useRef<VideoPlayerRef>(null);

  // [Order 1-2] WebSocket 연결 (실시간 진행률 수신)
  // - 커스텀 훅 useWebSocket을 호출하여 서버와의 실시간 통신을 설정한다.
  // - C 소켓 프로그래밍과 유사하지만, 브라우저의 WebSocket API를 추상화한 것이다.
  // - 관련 단계: [CG-v1.3.0] WebSocket 연동
  const { connected, progress } = useWebSocket();

  // [Order 2] 이벤트 핸들러 함수들
  // - 사용자 인터랙션이나 자식 컴포넌트의 콜백으로 호출되는 함수들이다.
  // - C의 콜백 함수 패턴과 동일한 개념이다.

  // [Order 2-1] 비디오 업로드 완료 핸들러
  // - VideoUpload 컴포넌트에서 업로드가 완료되면 호출된다.
  // - 업로드된 비디오 메타데이터로 상태를 초기화한다.
  const handleVideoUploaded = (uploadedVideo: VideoMetadata) => {
    setVideo(uploadedVideo);
    setCurrentTime(0);
    setDuration(0);
    setProcessedVideos([]);
  };

  // [Order 2-2] 타임라인 탐색(Seek) 핸들러
  // - Timeline 컴포넌트에서 사용자가 특정 시간을 클릭하면 호출된다.
  // - playerRef를 통해 VideoPlayer에 직접 명령을 전달한다 (명령형 제어).
  const handleSeek = (time: number) => {
    playerRef.current?.seekTo(time);
  };

  // [Order 2-3] 트림(Trim) 완료 핸들러
  // - EditPanel에서 비디오 트림이 완료되면 호출된다.
  // - 결과 비디오를 목록에 추가하고, 트림된 비디오를 현재 비디오로 설정한다.
  // - 관련 단계: [CG-v1.1.0] Trim/Split 기능
  const handleTrimComplete = (result: EditResult) => {
    setProcessedVideos([result]);
    // Convert EditResult to VideoMetadata format
    const trimmedVideo: VideoMetadata = {
      url: result.url,
      path: result.path,
      filename: result.filename,
      originalName: `Trimmed - ${video?.originalName || 'video'}`,
      size: result.size,
      mimetype: 'video/mp4',
      duration: result.duration,
    };
    setVideo(trimmedVideo);
  };

  // [Order 2-4] 분할(Split) 완료 핸들러
  // - 비디오가 여러 파트로 분할되면 호출된다.
  const handleSplitComplete = (results: EditResult[]) => {
    setProcessedVideos(results);
  };

  // [Order 2-5] 프로젝트 로드 핸들러
  // - ProjectPanel에서 저장된 프로젝트를 불러올 때 호출된다.
  // - 관련 단계: [CG-v1.3.0] 프로젝트 저장/불러오기
  const handleProjectLoad = (project: Project) => {
    const loadedVideo: VideoMetadata = {
      url: project.video_url,
      path: '',
      filename: project.video_filename,
      originalName: project.name,
      size: 0,
      mimetype: 'video/mp4',
    };
    setVideo(loadedVideo);
    setProcessedVideos([]);
  };

  // [Order 3] JSX 렌더링 (UI 선언부)
  // [LEARN] JSX란?
  // - JavaScript 안에서 HTML과 유사한 문법을 사용할 수 있게 하는 확장 문법이다.
  // - 컴파일 시 React.createElement() 호출로 변환된다.
  // - C로 비유하면, printf 대신 "템플릿"을 반환하는 것과 유사하다.
  //
  // [LEARN] 조건부 렌더링
  // - {조건 ? A : B} 또는 {조건 && A} 패턴으로 조건부 UI를 표현한다.
  // - C의 if-else와 동일한 로직이지만, JSX 내부에서 표현식으로 사용한다.
  return (
    <div className="min-h-screen bg-gray-900 text-white">
      <div className="container mx-auto p-8">
        <header className="mb-8">
          <h1 className="text-4xl font-bold mb-2">Video Editor</h1>
          <p className="text-gray-400">Web-Based Video Editor</p>
          <p className="text-sm text-gray-500 mt-2">Phase 1 - MVP 1.3: WebSocket + PostgreSQL</p>
          {connected && (
            <div className="mt-2 text-xs text-green-400">● WebSocket Connected</div>
          )}
        </header>

        <div className="space-y-6">
          {/* [LEARN] 조건부 렌더링: 비디오가 없으면 업로드 UI, 있으면 편집 UI */}
          {!video ? (
            <VideoUpload onVideoUploaded={handleVideoUploaded} />
          ) : (
            // [Order 3-1] 비디오 편집 레이아웃
            // - CSS Grid를 사용하여 2/3 : 1/3 비율로 레이아웃을 구성한다.
            // - lg:col-span-2는 Tailwind CSS 클래스로, 큰 화면에서 2열을 차지한다.
            <div className="grid grid-cols-1 lg:grid-cols-3 gap-6">
              <div className="lg:col-span-2 space-y-6">
                {/* [Order 3-2] VideoPlayer: ref로 직접 제어 가능한 비디오 플레이어 */}
                <VideoPlayer
                  ref={playerRef}
                  video={video}
                  onTimeUpdate={setCurrentTime}
                  onDurationChange={setDuration}
                />
                {/* [Order 3-3] Timeline: 비디오 진행 표시 및 탐색 UI */}
                <Timeline
                  duration={duration}
                  currentTime={currentTime}
                  onSeek={handleSeek}
                />

                {processedVideos.length > 0 && (
                  <div className="bg-gray-800 rounded-lg p-4">
                    <h3 className="text-lg font-semibold mb-3">Processed Videos</h3>
                    <div className="space-y-2">
                      {processedVideos.map((pv, index) => (
                        <div
                          key={pv.filename}
                          className="flex items-center justify-between p-3 bg-gray-700 rounded"
                        >
                          <div>
                            <div className="font-medium">
                              {processedVideos.length > 1 ? `Part ${index + 1}` : 'Trimmed Video'}
                            </div>
                            <div className="text-sm text-gray-400">
                              {(pv.size / (1024 * 1024)).toFixed(2)} MB
                              {pv.duration && ` • ${Math.floor(pv.duration)}s`}
                            </div>
                          </div>
                          <a
                            href={pv.url}
                            download={pv.filename}
                            className="px-3 py-1 bg-blue-600 hover:bg-blue-700 rounded text-sm transition-colors"
                          >
                            Download
                          </a>
                        </div>
                      ))}
                    </div>
                  </div>
                )}

                <button
                  onClick={() => setVideo(null)}
                  className="px-4 py-2 bg-gray-700 hover:bg-gray-600 rounded transition-colors"
                >
                  Upload New Video
                </button>
              </div>

              {/* [Order 3-4] 편집 패널 사이드바 */}
              <div className="lg:col-span-1 space-y-6">
                {/* EditPanel: 트림, 분할, 자막, 속도 조절 등 편집 기능 */}
                <EditPanel
                  video={video}
                  duration={duration}
                  currentTime={currentTime}
                  onEditComplete={handleTrimComplete}
                  onSplitComplete={handleSplitComplete}
                />

                {/* ProjectPanel: 프로젝트 저장/불러오기 기능 */}
                <ProjectPanel
                  video={video}
                  onProjectLoad={handleProjectLoad}
                />
              </div>
            </div>
          )}
        </div>

        {/* Progress Bar: WebSocket으로 수신한 실시간 진행률 표시 */}
        {progress && progress.progress < 100 && (
          <ProgressBar
            progress={progress.progress}
            operation={progress.operation}
            message={progress.message}
          />
        )}
      </div>
    </div>
  );
}

export default App;

// [Reader Notes]
// =============================================================================
// 이 파일에서 처음 등장한 개념: React 상태 관리, 컴포넌트 조합, Props 전달
//
// ## React 상태 관리 패턴 (C 개발자 관점)
// 1. **useState**: 렌더링 간에 유지되는 "로컬 상태"
//    - C의 static 지역 변수와 유사하지만, setter 호출 시 UI가 자동 갱신된다.
//
// 2. **useRef**: 렌더링에 영향을 주지 않는 "참조"
//    - C의 포인터와 유사. DOM 요소나 컴포넌트 인스턴스를 직접 참조한다.
//
// 3. **Props**: 부모 → 자식 데이터 전달
//    - C의 함수 인자와 동일. 컴포넌트에 데이터나 콜백을 전달한다.
//    - 예: <VideoPlayer video={video} onTimeUpdate={setCurrentTime} />
//
// ## 컴포넌트 구조
// App (루트)
// ├── VideoUpload (조건부: 비디오 없을 때)
// └── 편집 UI (조건부: 비디오 있을 때)
//     ├── VideoPlayer (ref로 제어)
//     ├── Timeline (seek 콜백)
//     ├── EditPanel (편집 완료 콜백)
//     └── ProjectPanel (프로젝트 로드 콜백)
//
// ## 이 파일을 이해한 다음, 이어서 보면 좋은 파일:
// 1. components/VideoUpload.tsx - 파일 업로드 UI와 드래그 앤 드롭
// 2. components/VideoPlayer.tsx - forwardRef 패턴과 useImperativeHandle
// 3. hooks/useWebSocket.ts - 커스텀 훅과 WebSocket 통신
// =============================================================================
