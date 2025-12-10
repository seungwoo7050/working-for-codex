// [FILE]
// - 목적: WebSocket 실시간 통신을 위한 커스텀 훅
// - 주요 역할: 서버와의 WebSocket 연결 관리, 진행률 메시지 수신, 자동 재연결
// - 관련 클론 가이드 단계: [CG-v1.3.0] WebSocket + 프로젝트 저장/불러오기
// - 권장 읽는 순서: Order 1 (타입) → Order 2 (연결 로직) → Order 3 (메시지 처리)
//
// [LEARN] C 개발자를 위한 WebSocket 이해:
// - WebSocket은 HTTP와 달리 "양방향 지속 연결"을 제공한다.
// - C의 TCP 소켓 프로그래밍과 유사하지만, 브라우저 API로 추상화되어 있다.
// - 연결 후에는 서버와 클라이언트가 언제든 메시지를 주고받을 수 있다.

import { useEffect, useState, useCallback, useRef } from 'react';

// [Order 1] 타입 정의
// - 서버로부터 수신하는 진행률 데이터 구조
interface ProgressData {
  operationId: string;
  operation: string;
  progress: number;
  message?: string;
}

// - WebSocket 메시지 타입 (공용체처럼 여러 타입을 정의)
interface WSMessage {
  type: 'progress' | 'complete' | 'error' | 'ping' | 'pong';
  data?: unknown;
}

// 환경 변수에서 WebSocket URL을 가져오거나 기본값 사용
const WS_URL = import.meta.env.VITE_WS_URL || 'ws://localhost:3001/ws';

// [Order 2] WebSocket 훅 정의
/**
 * WebSocket hook for real-time communication
 */
export function useWebSocket() {
  // [Order 2-1] 상태 및 참조
  // - connected: 연결 상태
  // - progress: 현재 수신 중인 진행률 데이터
  // - ws: WebSocket 인스턴스 참조 (렌더링 간 유지)
  // - reconnectTimeout: 재연결 타이머 참조
  const [connected, setConnected] = useState(false);
  const [progress, setProgress] = useState<ProgressData | null>(null);
  const ws = useRef<WebSocket | null>(null);
  const reconnectTimeout = useRef<number | null>(null);

  // [Order 2-2] 연결 함수 정의
  // [LEARN] useCallback으로 메모이제이션하여 useEffect 의존성 안정화
  const connect = useCallback(() => {
    try {
      // [LEARN] WebSocket 생성자
      // - new WebSocket(url)로 서버에 연결을 시도한다.
      // - C의 socket() + connect() 조합과 유사하다.
      const socket = new WebSocket(WS_URL);

      // [Order 3] WebSocket 이벤트 핸들러
      // - C의 select/poll 이벤트 루프 대신, 콜백 함수로 이벤트를 처리한다.
      
      // [Order 3-1] 연결 성공 이벤트
      socket.onopen = () => {
        setConnected(true);
      };

      // [Order 3-2] 메시지 수신 이벤트
      // - 서버로부터 메시지를 받을 때 호출된다.
      // - JSON 파싱 후 메시지 타입에 따라 분기 처리한다.
      socket.onmessage = (event) => {
        try {
          const message: WSMessage = JSON.parse(event.data);

          // [LEARN] switch 문으로 메시지 타입별 처리
          // - C의 switch와 동일하지만, 문자열 리터럴 타입으로 안전하게 분기
          switch (message.type) {
            case 'progress':
              // 진행률 업데이트
              setProgress(message.data as ProgressData);
              break;
            case 'complete':
              // 작업 완료 (현재는 특별한 처리 없음)
              break;
            case 'error':
              // 에러 발생 (현재는 특별한 처리 없음)
              break;
            case 'ping':
              // [LEARN] 하트비트(Heartbeat) 패턴
              // - 서버가 ping을 보내면 pong으로 응답하여 연결 유지를 확인한다.
              // - TCP keepalive와 유사한 목적이다.
              socket.send(JSON.stringify({ type: 'pong' }));
              break;
          }
        } catch {
          // JSON 파싱 에러는 무시
        }
      };

      // [Order 3-3] 연결 종료 이벤트
      // - 연결이 끊어지면 3초 후 자동 재연결을 시도한다.
      socket.onclose = () => {
        setConnected(false);

        // [LEARN] 자동 재연결 패턴
        // - 네트워크 불안정 시 자동으로 재연결을 시도한다.
        // - window.setTimeout으로 3초 후 재연결 시도를 예약한다.
        reconnectTimeout.current = window.setTimeout(() => {
          // eslint-disable-next-line react-hooks/immutability
          connect();
        }, 3000);
      };

      // 에러 발생 시 onclose에서 처리됨
      socket.onerror = () => {
        // Error handling is done in onclose
      };

      // ref에 소켓 인스턴스 저장
      ws.current = socket;
    } catch {
      // 연결 생성 자체가 실패한 경우 (URL 오류 등)
    }
  }, []);

  // [Order 4] 컴포넌트 마운트 시 연결, 언마운트 시 정리
  // [LEARN] useEffect 정리(cleanup) 함수
  // - 컴포넌트가 언마운트될 때 WebSocket 연결을 정리한다.
  // - C에서 리소스 해제(close, free)를 반드시 해야 하듯, React에서도 동일하다.
  useEffect(() => {
    connect();

    // 정리 함수: 타이머와 소켓 연결 해제
    return () => {
      if (reconnectTimeout.current) {
        clearTimeout(reconnectTimeout.current);
      }
      if (ws.current) {
        ws.current.close();
      }
    };
  }, [connect]);

  return {
    connected,
    progress,
  };
}

// [Reader Notes]
// =============================================================================
// 이 파일에서 처음 등장한 개념: WebSocket, 자동 재연결, 하트비트
//
// ## WebSocket vs HTTP (C 개발자 관점)
// | 특성 | HTTP | WebSocket |
// |------|------|-----------|
// | 연결 | 요청마다 새 연결 | 지속 연결 |
// | 방향 | 클라이언트 → 서버 | 양방향 |
// | 오버헤드 | 헤더 매번 전송 | 최초 핸드셰이크만 |
// | 사용 사례 | REST API | 실시간 알림, 채팅 |
//
// ## 재연결 전략
// - onclose에서 setTimeout으로 재연결 예약
// - 지수 백오프(exponential backoff)를 적용하면 더 좋음
// - 현재는 단순히 3초 후 재시도
//
// ## useRef로 인스턴스 유지하는 이유
// - useState와 달리 ref.current 변경은 리렌더링을 유발하지 않음
// - WebSocket 객체는 렌더링과 무관하게 유지되어야 함
// - C에서 전역 변수로 소켓 fd를 유지하는 것과 유사
//
// ## 이 파일을 이해한 다음, 이어서 보면 좋은 파일:
// 1. backend/src/ws/websocket.service.ts - 서버 측 WebSocket 구현
// 2. hooks/useProjects.ts - 프로젝트 저장/불러오기 훅
// =============================================================================
