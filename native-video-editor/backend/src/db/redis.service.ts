// [FILE]
// - 목적: Redis 캐싱 및 세션 관리 서비스
// - 주요 역할: 세션 저장/조회, 메타데이터 캐싱, 키-값 저장
// - 관련 클론 가이드 단계: [CG-v1.3.0] WebSocket/영속화
// - 권장 읽는 순서: database.service.ts와 함께
//
// [LEARN] C 개발자를 위한 Redis:
// - Redis는 인메모리 키-값 저장소로, C로 작성되어 매우 빠르다.
// - 디스크 DB(PostgreSQL)보다 훨씬 빠른 읽기/쓰기가 필요할 때 사용한다.
// - 세션, 캐시, 실시간 데이터 등 휘발성 데이터에 적합하다.
// - C에서 memcached를 사용하는 것과 유사한 목적이다.
//
// [Reader Notes]
// - ioredis: Node.js용 Redis 클라이언트 라이브러리
// - setex: SET + EXPIRE를 원자적으로 실행 (TTL 설정)
// - lazyConnect: 첫 명령 시 연결 (즉시 연결하지 않음)

import { Redis } from 'ioredis';

/**
 * Redis connection and session management
 * Based on cpp-pvp-server M1.8 pattern
 */
// [LEARN] 클래스 기반 서비스 패턴
// - Redis 클라이언트 인스턴스를 관리한다.
// - 세션 관련 메서드들을 그룹화하여 제공한다.
export class RedisService {
  private client: Redis;
  private readonly SESSION_TTL = 3600; // 1 hour in seconds

  // [LEARN] Redis 클라이언트 초기화
  // - retryStrategy: 연결 실패 시 재시도 전략 (지수 백오프)
  // - lazyConnect: 첫 명령 시까지 연결을 지연한다.
  constructor() {
    this.client = new Redis({
      host: process.env.REDIS_HOST || 'localhost',
      port: parseInt(process.env.REDIS_PORT || '6379'),
      password: process.env.REDIS_PASSWORD,
      retryStrategy: (times: number) => {
        // [LEARN] 지수 백오프(Exponential Backoff)
        // - 재시도 간격을 점점 늘려서 서버 부하를 줄인다.
        const delay = Math.min(times * 50, 2000);
        return delay;
      },
      lazyConnect: true, // Don't connect immediately
    });

    this.client.on('error', (err: Error) => {
      console.error('Redis error:', err);
    });

    this.client.on('connect', () => {
      console.log('Redis connected');
    });
  }

  /**
   * Set a key-value pair with TTL
   */
  // [LEARN] setex: SET + EXPIRE
  // - 키를 설정하면서 동시에 만료 시간(TTL)을 지정한다.
  // - 세션이나 캐시처럼 일정 시간 후 자동 삭제가 필요할 때 사용.
  async set(key: string, value: string, ttl?: number): Promise<void> {
    const expiry = ttl || this.SESSION_TTL;
    await this.client.setex(key, expiry, value);
  }

  /**
   * Get a value by key
   */
  async get(key: string): Promise<string | null> {
    return await this.client.get(key);
  }

  /**
   * Delete a key
   */
  async delete(key: string): Promise<void> {
    await this.client.del(key);
  }

  /**
   * Check if key exists
   */
  async exists(key: string): Promise<boolean> {
    const result = await this.client.exists(key);
    return result === 1;
  }

  /**
   * Set session data
   */
  async setSession(sessionId: string, data: object): Promise<void> {
    await this.set(`session:${sessionId}`, JSON.stringify(data), this.SESSION_TTL);
  }

  /**
   * Get session data
   */
  async getSession(sessionId: string): Promise<object | null> {
    const data = await this.get(`session:${sessionId}`);
    return data ? JSON.parse(data) : null;
  }

  /**
   * Extend session TTL
   */
  async extendSession(sessionId: string): Promise<void> {
    await this.client.expire(`session:${sessionId}`, this.SESSION_TTL);
  }

  /**
   * Delete session
   */
  async deleteSession(sessionId: string): Promise<void> {
    await this.delete(`session:${sessionId}`);
  }

  /**
   * Cache video metadata
   */
  async cacheMetadata(filename: string, metadata: object): Promise<void> {
    await this.set(`metadata:${filename}`, JSON.stringify(metadata), 3600);
  }

  /**
   * Get cached metadata
   */
  async getCachedMetadata(filename: string): Promise<object | null> {
    const data = await this.get(`metadata:${filename}`);
    return data ? JSON.parse(data) : null;
  }

  /**
   * Close the connection
   */
  async close(): Promise<void> {
    await this.client.quit();
  }
}

// Singleton instance
export const redis = new RedisService();
