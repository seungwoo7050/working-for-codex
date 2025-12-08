// [FILE]
// - 목적: PostgreSQL 데이터베이스 연결 및 쿼리 서비스
// - 주요 역할: 커넥션 풀 관리, 파라미터화된 쿼리, 트랜잭션 처리
// - 관련 클론 가이드 단계: [CG-v1.3.0] WebSocket/영속화
// - 권장 읽는 순서: redis.service.ts와 함께
//
// [LEARN] C 개발자를 위한 PostgreSQL 연결:
// - pg 라이브러리는 PostgreSQL C 라이브러리(libpq)의 Node.js 바인딩이다.
// - 커넥션 풀(Pool)은 미리 여러 연결을 만들어두고 재사용한다.
// - C에서 mysql_real_connect() 후 연결을 재사용하는 것과 유사하다.
//
// [Reader Notes]
// - Pool: 여러 클라이언트 연결을 관리하는 객체
// - PoolClient: 풀에서 빌려온 단일 연결
// - query()는 SQL Injection 방지를 위해 파라미터화된 쿼리를 사용한다.

import { Pool, PoolClient } from 'pg';

/**
 * PostgreSQL connection pool configuration
 * Based on cpp-pvp-server M1.10 pattern
 */
// [LEARN] 클래스 기반 서비스 패턴
// - 데이터베이스 연결 상태(pool)를 인스턴스 변수로 관리한다.
// - 싱글톤으로 사용하여 애플리케이션 전체에서 같은 풀을 공유한다.
export class DatabaseService {
  private pool: Pool;

  // [LEARN] 생성자에서 풀 초기화
  // - 환경 변수에서 연결 정보를 가져온다.
  // - max: 동시에 열 수 있는 최대 연결 수 (기본 20)
  // - idleTimeoutMillis: 유휴 연결 자동 해제 시간
  constructor() {
    this.pool = new Pool({
      host: process.env.POSTGRES_HOST || 'localhost',
      port: parseInt(process.env.POSTGRES_PORT || '5432'),
      database: process.env.POSTGRES_DB || 'video-editor',
      user: process.env.POSTGRES_USER || 'postgres',
      password: process.env.POSTGRES_PASSWORD || 'postgres',
      max: 20, // Maximum pool size
      idleTimeoutMillis: 30000,
      connectionTimeoutMillis: 2000,
    });

    // Handle pool errors
    this.pool.on('error', (err) => {
      console.error('Unexpected error on idle client', err);
    });
  }

  /**
   * Get pool instance
   */
  getPool(): Pool {
    return this.pool;
  }

  /**
   * Execute a query with parameterized values (SQL injection prevention)
   */
  // [LEARN] 파라미터화된 쿼리
  // - SQL 문자열과 값을 분리하여 SQL Injection을 방지한다.
  // - C에서 prepared statement를 사용하는 것과 동일한 패턴이다.
  // - 예: query('SELECT * FROM users WHERE id = $1', [userId])
  async query<T = unknown>(text: string, params?: unknown[]): Promise<T[]> {
    // [LEARN] 풀에서 클라이언트 빌려오기
    // - connect()로 연결을 획득하고, 작업 후 release()로 반환한다.
    // - finally 블록에서 반환하여 예외 발생 시에도 누수를 방지한다.
    const client = await this.pool.connect();
    try {
      const result = await client.query(text, params);
      return result.rows as T[];
    } finally {
      client.release();
    }
  }

  /**
   * Execute a query and return a single row
   */
  async queryOne<T = unknown>(text: string, params?: unknown[]): Promise<T | null> {
    const rows = await this.query<T>(text, params);
    return rows.length > 0 ? rows[0] : null;
  }

  /**
   * Execute a transaction
   */
  // [LEARN] 트랜잭션 패턴
  // - BEGIN으로 시작, 성공 시 COMMIT, 실패 시 ROLLBACK
  // - C에서 mysql_autocommit(false) 후 수동 커밋하는 것과 동일
  // - 콜백 함수에서 여러 쿼리를 하나의 트랜잭션으로 묶는다.
  async transaction<T>(callback: (client: PoolClient) => Promise<T>): Promise<T> {
    const client = await this.pool.connect();
    try {
      await client.query('BEGIN');
      const result = await callback(client);
      await client.query('COMMIT');
      return result;
    } catch (error) {
      await client.query('ROLLBACK');
      throw error;
    } finally {
      client.release();
    }
  }

  /**
   * Close the connection pool
   */
  async close(): Promise<void> {
    await this.pool.end();
  }

  /**
   * Run migrations
   */
  async runMigrations(): Promise<void> {
    const fs = await import('fs/promises');
    const path = await import('path');

    const migrationsDir = path.resolve(process.cwd(), '..', 'migrations');

    try {
      const files = await fs.readdir(migrationsDir);
      const sqlFiles = files.filter(f => f.endsWith('.sql')).sort();

      for (const file of sqlFiles) {
        const filePath = path.join(migrationsDir, file);
        const sql = await fs.readFile(filePath, 'utf-8');

        console.log(`Running migration: ${file}`);
        await this.pool.query(sql);
        console.log(`✓ Migration ${file} completed`);
      }
    } catch (error) {
      console.error('Migration error:', error);
      throw error;
    }
  }
}

// Singleton instance
export const db = new DatabaseService();
