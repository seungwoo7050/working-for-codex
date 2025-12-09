import { useEffect, useMemo, useState } from 'react';
import { createJob, listJobs } from './api';
import type { Job } from './types';

// [FILE]
// - 목적: 대시보드 메인 화면을 구성한다.
// - 주요 역할: 잡 목록을 로드하고 생성 폼, 상태 배지를 렌더링한다.
// - 관련 토이 버전: [FE-F0.1]~[FE-F0.4]
// - 권장 읽는 순서: 상태 정의 -> 데이터 로드 useEffect -> 렌더링.
//
// [LEARN] React 훅을 활용해 비동기 데이터 로딩과 폼 처리를 구현하는 패턴을 익힌다.
function statusColor(status: string): string {
  switch (status) {
    case 'DONE':
      return '#16a34a';
    case 'FAILED':
      return '#dc2626';
    case 'RUNNING':
      return '#2563eb';
    default:
      return '#6b7280';
  }
}

export default function App() {
  const [jobs, setJobs] = useState<Job[]>([]);
  const [title, setTitle] = useState('');
  const [error, setError] = useState<string | null>(null);
  const [loading, setLoading] = useState(false);

  const sortedJobs = useMemo(() =>
    [...jobs].sort((a, b) => b.id - a.id),
  [jobs]);

  // [Order 1] 최초 렌더 시 목록을 불러온다.
  // [LEARN] useEffect와 async 함수 조합으로 초기 데이터를 로드한다.
  useEffect(() => {
    refresh();
    const timer = setInterval(() => refresh(true), 5000);
    return () => clearInterval(timer);
  }, []);

  async function refresh(silent = false) {
    if (!silent) setLoading(true);
    try {
      const data = await listJobs();
      setJobs(data);
      setError(null);
    } catch (err) {
      setError('목록을 불러오지 못했습니다');
    } finally {
      setLoading(false);
    }
  }

  async function handleSubmit(event: React.FormEvent<HTMLFormElement>) {
    event.preventDefault();
    if (!title.trim()) return;
    try {
      setLoading(true);
      await createJob(title.trim());
      setTitle('');
      await refresh(true);
    } catch (err) {
      setError('작업 생성에 실패했습니다');
    } finally {
      setLoading(false);
    }
  }

  return (
    <main style={{ fontFamily: 'Inter, system-ui', maxWidth: 720, margin: '0 auto', padding: '24px' }}>
      <h1>Mini Job Dashboard</h1>
      <p>백엔드 mini-job-service와 연결해 작업을 생성/조회합니다.</p>

      <form onSubmit={handleSubmit} style={{ marginBottom: '16px' }}>
        <label>
          새 작업 제목
          <input
            value={title}
            onChange={(e) => setTitle(e.target.value)}
            placeholder="예: 데이터 동기화"
            style={{ marginLeft: 8 }}
          />
        </label>
        <button type="submit" style={{ marginLeft: 8 }} disabled={loading}>추가</button>
      </form>

      {error && <div style={{ color: '#dc2626', marginBottom: 8 }}>{error}</div>}
      {loading && <div style={{ color: '#2563eb', marginBottom: 8 }}>로딩 중...</div>}

      <section>
        {sortedJobs.length === 0 ? (
          <p>등록된 작업이 없습니다.</p>
        ) : (
          <ul style={{ listStyle: 'none', padding: 0 }}>
            {sortedJobs.map((job) => (
              <li key={job.id} style={{ border: '1px solid #e5e7eb', padding: '12px', marginBottom: '8px', borderRadius: '8px' }}>
                <div style={{ display: 'flex', justifyContent: 'space-between' }}>
                  <strong>{job.title}</strong>
                  <span style={{ color: statusColor(job.status) }}>{job.status}</span>
                </div>
                <div style={{ fontSize: 12, color: '#6b7280' }}>생성 시각: {job.createdAt}</div>
              </li>
            ))}
          </ul>
        )}
      </section>
    </main>
  );
}
