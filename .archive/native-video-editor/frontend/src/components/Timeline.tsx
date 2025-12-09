// [FILE]
// - 목적: Canvas 기반 비디오 타임라인 UI 컴포넌트
// - 주요 역할: 눈금자(Ruler) 렌더링, 플레이헤드 표시, 시간 탐색(Seek)
// - 관련 클론 가이드 단계: [CG-v3.1.0] 시각적 편집 UI
// - 권장 읽는 순서: Order 1 (Canvas 설정) → Order 2 (눈금자) → Order 3 (플레이헤드) → Order 4 (클릭 핸들링)
//
// [LEARN] C 개발자를 위한 Canvas 2D 그래픽:
// - HTML5 Canvas는 C의 프레임버퍼에 직접 그리는 것과 유사하다.
// - ctx.beginPath(), ctx.moveTo(), ctx.lineTo(), ctx.stroke()로 선을 그린다.
// - ctx.fillRect()로 사각형을 채우고, ctx.arc()로 원을 그린다.
// - 좌표계는 좌상단이 (0,0)이다.
//
// [Reader Notes]
// - Canvas 2D API는 즉시 모드(Immediate Mode) 그래픽이다.
// - 매 프레임마다 전체를 다시 그려야 한다 (Retained Mode인 DOM과 다름).
// - devicePixelRatio(DPR)를 고려하지 않으면 고해상도 디스플레이에서 흐릿해진다.

import { useRef, useEffect } from 'react';

// [LEARN] TypeScript 인터페이스
// - C의 구조체(struct)와 유사하지만 런타임에 존재하지 않는다 (컴파일 타임만).
// - 컴포넌트가 받을 props의 타입을 명시한다.
interface TimelineProps {
  duration: number;      // 비디오 전체 길이 (초)
  currentTime: number;   // 현재 재생 위치 (초)
  onSeek?: (time: number) => void;  // 시간 탐색 콜백 (선택적)
}

/**
 * Canvas-based timeline component with ruler and time markers
 */
export function Timeline({ duration, currentTime, onSeek }: TimelineProps) {
  // [LEARN] useRef: DOM 요소 참조
  // - canvasRef.current로 실제 <canvas> 요소에 접근한다.
  // - C에서 포인터로 메모리 주소를 저장하는 것과 유사하다.
  const canvasRef = useRef<HTMLCanvasElement>(null);
  const containerRef = useRef<HTMLDivElement>(null);

  // [Order 1] Canvas 설정 및 렌더링
  useEffect(() => {
    const canvas = canvasRef.current;
    const container = containerRef.current;
    if (!canvas || !container) return;

    // [LEARN] Canvas 2D 컨텍스트 획득
    // - getContext('2d')로 2D 드로잉 API를 얻는다.
    // - '2d' 외에 'webgl', 'webgl2' 등도 있다.
    const ctx = canvas.getContext('2d');
    if (!ctx) return;

    // [LEARN] 고해상도 디스플레이 대응 (DPR 처리)
    // - Retina 디스플레이는 devicePixelRatio가 2 이상이다.
    // - Canvas 크기를 DPR만큼 확대하고, CSS로 원래 크기로 축소한다.
    // - 이렇게 하면 선명한 그래픽을 얻을 수 있다.
    // Set canvas size to match container
    const dpr = window.devicePixelRatio || 1;
    const rect = container.getBoundingClientRect();
    canvas.width = rect.width * dpr;   // 실제 픽셀 크기 (확대)
    canvas.height = 80 * dpr;
    canvas.style.width = `${rect.width}px`;  // CSS 크기 (원래 크기)
    canvas.style.height = '80px';

    // [LEARN] 스케일 조정
    // - ctx.scale(dpr, dpr)로 모든 드로잉 명령을 확대한다.
    // - 이후 코드에서는 CSS 픽셀 단위로 작업하면 된다.
    ctx.scale(dpr, dpr);

    // Clear canvas
    ctx.clearRect(0, 0, rect.width, 80);

    // Background
    ctx.fillStyle = '#1f2937';
    ctx.fillRect(0, 0, rect.width, 80);

    if (duration <= 0) return;

    // [Order 2] 눈금자(Ruler) 그리기
    // Draw ruler
    ctx.strokeStyle = '#4b5563';  // 선 색상
    ctx.fillStyle = '#9ca3af';    // 텍스트 색상
    ctx.font = '10px sans-serif';

    // [LEARN] 동적 간격 계산
    // - 영상 길이에 따라 눈금 간격을 조절한다.
    // - 60초 이상: 10초 간격, 30~60초: 5초 간격, 30초 미만: 1초 간격
    const pixelsPerSecond = rect.width / duration;
    const interval = duration > 60 ? 10 : duration > 30 ? 5 : 1;

    for (let time = 0; time <= duration; time += interval) {
      const x = time * pixelsPerSecond;

      // [LEARN] Canvas 경로(Path) 기반 드로잉
      // - beginPath(): 새 경로 시작
      // - moveTo(): 펜 이동 (선 그리지 않음)
      // - lineTo(): 선 그리기
      // - stroke(): 실제로 선을 화면에 출력
      // Major tick
      ctx.beginPath();
      ctx.moveTo(x, 60);
      ctx.lineTo(x, 80);
      ctx.stroke();

      // Time label
      const mins = Math.floor(time / 60);
      const secs = Math.floor(time % 60);
      const label = mins > 0 ? `${mins}:${secs.toString().padStart(2, '0')}` : `${secs}s`;
      ctx.fillText(label, x + 2, 55);
    }

    // Draw minor ticks (보조 눈금)
    const minorInterval = interval / 5;
    for (let time = 0; time <= duration; time += minorInterval) {
      const x = time * pixelsPerSecond;

      if (time % interval !== 0) {
        ctx.beginPath();
        ctx.moveTo(x, 70);
        ctx.lineTo(x, 80);
        ctx.globalAlpha = 0.3;  // 투명도 조절
        ctx.stroke();
        ctx.globalAlpha = 1;    // 투명도 복원
      }
    }

    // [Order 3] 플레이헤드(현재 위치) 그리기
    // Draw playhead
    const playheadX = currentTime * pixelsPerSecond;
    ctx.strokeStyle = '#3b82f6';  // 파란색
    ctx.lineWidth = 2;            // 선 두께
    ctx.beginPath();
    ctx.moveTo(playheadX, 0);
    ctx.lineTo(playheadX, 80);
    ctx.stroke();

    // [LEARN] 원 그리기: arc(x, y, radius, startAngle, endAngle)
    // - 라디안 단위 사용 (Math.PI * 2 = 360도)
    // Playhead circle
    ctx.fillStyle = '#3b82f6';
    ctx.beginPath();
    ctx.arc(playheadX, 5, 5, 0, Math.PI * 2);
    ctx.fill();

  }, [duration, currentTime]);  // duration 또는 currentTime 변경 시 다시 렌더링

  // [Order 4] 클릭으로 시간 탐색
  const handleClick = (e: React.MouseEvent<HTMLCanvasElement>) => {
    if (!canvasRef.current || duration <= 0 || !onSeek) return;

    // [LEARN] 클릭 위치를 시간으로 변환
    // - e.clientX: 뷰포트 기준 마우스 X 좌표
    // - rect.left: Canvas 요소의 뷰포트 기준 왼쪽 좌표
    // - 차이가 Canvas 내부 X 좌표가 된다.
    const rect = canvasRef.current.getBoundingClientRect();
    const x = e.clientX - rect.left;
    const time = (x / rect.width) * duration;  // 비율로 시간 계산
    // [LEARN] 범위 제한: Math.max/Math.min으로 0~duration 사이로 클램프
    onSeek(Math.max(0, Math.min(duration, time)));
  };

  return (
    <div className="bg-gray-800 rounded-lg p-4">
      <h3 className="text-sm font-semibold mb-2 text-gray-300">Timeline</h3>
      <div ref={containerRef} className="w-full">
        <canvas
          ref={canvasRef}
          onClick={handleClick}
          className="cursor-pointer rounded"
        />
      </div>
    </div>
  );
}
