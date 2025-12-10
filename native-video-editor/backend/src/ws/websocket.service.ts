// [FILE]
// - 목적: 실시간 양방향 통신을 위한 WebSocket 서비스
// - 주요 역할: 클라이언트 연결 관리, 진행률 브로드캐스트, 핑/퐁 유지
// - 관련 클론 가이드 단계: [CG-v1.3.0] WebSocket/영속화
// - 권장 읽는 순서: server.ts 이후
//
// [LEARN] C 개발자를 위한 WebSocket:
// - HTTP는 요청-응답 방식이지만, WebSocket은 양방향 통신이 가능하다.
// - 소켓 연결을 유지하며 서버와 클라이언트가 자유롭게 메시지를 주고받는다.
// - C에서 TCP 소켓 프로그래밍과 유사하지만, HTTP 업그레이드로 시작한다.
// - 진행률 표시, 채팅, 실시간 알림 등에 사용된다.
//
// [Reader Notes]
// - ws 라이브러리: Node.js용 WebSocket 구현
// - 각 클라이언트에 UUID를 할당하여 구분한다.
// - pingInterval로 연결 상태를 확인하고 끊어진 연결을 정리한다.

import { WebSocketServer, WebSocket } from 'ws';
import { Server as HTTPServer } from 'http';
import { v4 as uuidv4 } from 'uuid';

/**
 * WebSocket message types
 */
// [LEARN] 메시지 타입 정의
// - type 필드로 메시지 종류를 구분한다.
// - data는 unknown (any와 유사하지만 더 안전) 타입으로 유연하게 정의.
export interface WSMessage {
  type: 'progress' | 'complete' | 'error' | 'ping' | 'pong';
  data?: unknown;
}

/**
 * Progress data
 */
// [LEARN] 진행률 데이터 구조
// - 어떤 작업(trim, split 등)의 진행률을 클라이언트에 전달한다.
export interface ProgressData {
  operationId: string;                              // 작업 식별자
  operation: 'trim' | 'split' | 'process' | 'upload';  // 작업 종류
  progress: number; // 0-100                        // 진행률 (%)
  message?: string;                                 // 추가 메시지
}

/**
 * WebSocket service for real-time communication
 * Based on cpp-pvp-server M1.6 pattern
 */
export class WebSocketService {
  private wss: WebSocketServer;                    // WebSocket 서버 인스턴스
  private clients: Map<string, WebSocket> = new Map();  // 연결된 클라이언트 맵
  private pingInterval: NodeJS.Timeout | null = null;  // 핑 타이머

  constructor(server: HTTPServer) {
    // [LEARN] WebSocket 서버 생성
    // - HTTP 서버에 WebSocket을 붙인다 (같은 포트 사용).
    // - path: '/ws'로 접근 경로를 지정한다.
    this.wss = new WebSocketServer({ server, path: '/ws' });

    // [LEARN] 연결 이벤트 핸들링
    // - 'connection' 이벤트: 새 클라이언트가 연결될 때 발생.
    // - ws 객체로 해당 클라이언트와 통신한다.
    this.wss.on('connection', (ws: WebSocket) => {
      const clientId = uuidv4();  // UUID로 클라이언트 식별
      this.clients.set(clientId, ws);

      console.log(`WebSocket client connected: ${clientId}`);
      console.log(`Total clients: ${this.clients.size}`);

      // Send welcome message
      this.send(ws, {
        type: 'ping',
        data: { clientId, message: 'Connected to native-video-editor WebSocket' },
      });

      // [LEARN] 메시지 수신 이벤트
      // - 클라이언트가 보낸 메시지를 JSON으로 파싱한다.
      // Handle messages from client
      ws.on('message', (data: Buffer) => {
        try {
          const message = JSON.parse(data.toString()) as WSMessage;
          this.handleMessage(clientId, ws, message);
        } catch (error) {
          console.error('Failed to parse WebSocket message:', error);
        }
      });

      // [LEARN] 연결 종료 이벤트
      // - 클라이언트가 연결을 끊으면 맵에서 제거한다.
      // Handle client disconnect
      ws.on('close', () => {
        this.clients.delete(clientId);
        console.log(`WebSocket client disconnected: ${clientId}`);
        console.log(`Total clients: ${this.clients.size}`);
      });

      // Handle errors
      ws.on('error', (error) => {
        console.error(`WebSocket error for client ${clientId}:`, error);
        this.clients.delete(clientId);
      });
    });

    // Start ping interval to keep connections alive
    this.startPingInterval();
  }

  /**
   * Handle incoming messages
   */
  private handleMessage(clientId: string, ws: WebSocket, message: WSMessage): void {
    switch (message.type) {
      case 'ping':
        this.send(ws, { type: 'pong', data: { timestamp: Date.now() } });
        break;
      case 'pong':
        // Client responded to ping
        break;
      default:
        console.log(`Unknown message type from ${clientId}:`, message.type);
    }
  }

  /**
   * Send message to a specific client
   */
  private send(ws: WebSocket, message: WSMessage): void {
    if (ws.readyState === WebSocket.OPEN) {
      ws.send(JSON.stringify(message));
    }
  }

  /**
   * Broadcast progress to all clients
   */
  broadcastProgress(data: ProgressData): void {
    const message: WSMessage = {
      type: 'progress',
      data,
    };

    this.broadcast(message);
  }

  /**
   * Broadcast completion message
   */
  broadcastComplete(operationId: string, result: unknown): void {
    const message: WSMessage = {
      type: 'complete',
      data: { operationId, result },
    };

    this.broadcast(message);
  }

  /**
   * Broadcast error message
   */
  broadcastError(operationId: string, error: string): void {
    const message: WSMessage = {
      type: 'error',
      data: { operationId, error },
    };

    this.broadcast(message);
  }

  /**
   * Broadcast message to all connected clients
   */
  private broadcast(message: WSMessage): void {
    const messageStr = JSON.stringify(message);

    this.clients.forEach((ws) => {
      if (ws.readyState === WebSocket.OPEN) {
        ws.send(messageStr);
      }
    });
  }

  /**
   * Send ping to all clients periodically
   */
  private startPingInterval(): void {
    this.pingInterval = setInterval(() => {
      this.clients.forEach((ws, clientId) => {
        if (ws.readyState === WebSocket.OPEN) {
          this.send(ws, { type: 'ping', data: { timestamp: Date.now() } });
        } else {
          // Remove dead connections
          this.clients.delete(clientId);
        }
      });
    }, 30000); // Ping every 30 seconds
  }

  /**
   * Get the number of connected clients
   */
  getClientCount(): number {
    return this.clients.size;
  }

  /**
   * Close all connections
   */
  close(): void {
    if (this.pingInterval) {
      clearInterval(this.pingInterval);
    }

    this.clients.forEach((ws) => {
      ws.close();
    });

    this.wss.close();
  }
}
