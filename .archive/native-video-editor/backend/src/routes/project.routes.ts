// [FILE]
// - 목적: 프로젝트 CRUD API 엔드포인트
// - 주요 역할: 프로젝트 생성, 조회, 수정, 삭제
// - 관련 클론 가이드 단계: [CG-v1.3.0] WebSocket/영속화
// - 권장 읽는 순서: database.service.ts 이후
//
// [LEARN] C 개발자를 위한 REST API CRUD:
// - Create: POST /api/projects
// - Read: GET /api/projects, GET /api/projects/:id
// - Update: PUT /api/projects/:id
// - Delete: DELETE /api/projects/:id
// - C에서 CRUD 함수를 만드는 것과 같지만, HTTP 요청으로 호출된다.

import { Router, Request, Response } from 'express';
import { db } from '../db/database.service.js';

const router = Router();

/**
 * Project interface
 */
// [LEARN] 인터페이스로 DB 테이블 행(row) 타입 정의
// - snake_case: PostgreSQL 컬럼 명명 규칙과 일치
interface Project {
  id: number;
  name: string;
  description: string | null;
  video_filename: string;
  video_url: string;
  timeline_state: object;
  created_at: Date;
  updated_at: Date;
}

/**
 * Create a new project
 * POST /api/projects
 */
// [LEARN] Create 작업
// - INSERT 쿼리로 새 레코드를 생성한다.
// - RETURNING *로 생성된 레코드를 반환한다 (PostgreSQL 전용).
router.post('/', async (req: Request, res: Response): Promise<void> => {
  try {
    const { name, description, videoFilename, videoUrl, timelineState } = req.body;

    // 입력 검증
    if (!name || !videoFilename || !videoUrl) {
      res.status(400).json({
        error: 'Missing required fields: name, videoFilename, videoUrl',
      });
      return;
    }

    // [LEARN] 파라미터화된 쿼리
    // - $1, $2... 플레이스홀더로 SQL Injection 방지
    const project = await db.queryOne<Project>(
      `INSERT INTO projects (name, description, video_filename, video_url, timeline_state)
       VALUES ($1, $2, $3, $4, $5)
       RETURNING *`,
      [name, description || null, videoFilename, videoUrl, JSON.stringify(timelineState || {})]
    );

    res.status(201).json(project);
  } catch (error) {
    console.error('Create project error:', error);
    res.status(500).json({ error: 'Failed to create project' });
  }
});

/**
 * Get all projects
 * GET /api/projects
 */
// [LEARN] Read 작업 (목록)
// - SELECT 쿼리로 모든 레코드를 조회한다.
// - ORDER BY로 최신순 정렬한다.
router.get('/', async (req: Request, res: Response): Promise<void> => {
  try {
    const projects = await db.query<Project>(
      'SELECT * FROM projects ORDER BY updated_at DESC'
    );

    res.json(projects);
  } catch (error) {
    console.error('Get projects error:', error);
    res.status(500).json({ error: 'Failed to get projects' });
  }
});

/**
 * Get a single project by ID
 * GET /api/projects/:id
 */
// [LEARN] Read 작업 (단일)
// - URL 파라미터 :id로 특정 레코드를 조회한다.
router.get('/:id', async (req: Request, res: Response): Promise<void> => {
  try {
    const { id } = req.params;

    const project = await db.queryOne<Project>(
      'SELECT * FROM projects WHERE id = $1',
      [id]
    );

    if (!project) {
      res.status(404).json({ error: 'Project not found' });
      return;
    }

    res.json(project);
  } catch (error) {
    console.error('Get project error:', error);
    res.status(500).json({ error: 'Failed to get project' });
  }
});

/**
 * Update a project
 * PUT /api/projects/:id
 */
// [LEARN] Update 작업
// - UPDATE 쿼리로 기존 레코드를 수정한다.
router.put('/:id', async (req: Request, res: Response): Promise<void> => {
  try {
    const { id } = req.params;
    const { name, description, timelineState } = req.body;

    // Build update query dynamically
    const updates: string[] = [];
    const values: unknown[] = [];
    let paramIndex = 1;

    if (name !== undefined) {
      updates.push(`name = $${paramIndex++}`);
      values.push(name);
    }

    if (description !== undefined) {
      updates.push(`description = $${paramIndex++}`);
      values.push(description);
    }

    if (timelineState !== undefined) {
      updates.push(`timeline_state = $${paramIndex++}`);
      values.push(JSON.stringify(timelineState));
    }

    if (updates.length === 0) {
      res.status(400).json({ error: 'No fields to update' });
      return;
    }

    values.push(id);

    const project = await db.queryOne<Project>(
      `UPDATE projects SET ${updates.join(', ')}
       WHERE id = $${paramIndex}
       RETURNING *`,
      values
    );

    if (!project) {
      res.status(404).json({ error: 'Project not found' });
      return;
    }

    res.json(project);
  } catch (error) {
    console.error('Update project error:', error);
    res.status(500).json({ error: 'Failed to update project' });
  }
});

/**
 * Delete a project
 * DELETE /api/projects/:id
 */
router.delete('/:id', async (req: Request, res: Response): Promise<void> => {
  try {
    const { id } = req.params;

    const project = await db.queryOne<Project>(
      'DELETE FROM projects WHERE id = $1 RETURNING *',
      [id]
    );

    if (!project) {
      res.status(404).json({ error: 'Project not found' });
      return;
    }

    res.json({ message: 'Project deleted', project });
  } catch (error) {
    console.error('Delete project error:', error);
    res.status(500).json({ error: 'Failed to delete project' });
  }
});

export default router;
