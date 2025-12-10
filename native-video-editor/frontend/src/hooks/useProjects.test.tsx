import { renderHook, act } from '@testing-library/react';
import { useProjects } from './useProjects';

describe('useProjects', () => {
  const baseProject = {
    id: 1,
    name: 'Demo Project',
    description: 'desc',
    video_filename: 'video.mp4',
    video_url: 'http://localhost/video.mp4',
    timeline_state: { currentTime: 0, duration: 120 },
    created_at: '2024-01-01',
    updated_at: '2024-01-02',
  };

  beforeEach(() => {
    vi.restoreAllMocks();
  });

  it('creates project successfully and clears loading state', async () => {
    const fetchMock = vi.spyOn(global, 'fetch').mockResolvedValue({
      ok: true,
      json: vi.fn().mockResolvedValue(baseProject),
    } as unknown as Response);

    const { result } = renderHook(() => useProjects());
    let created = null;

    await act(async () => {
      created = await result.current.createProject({
        name: baseProject.name,
        description: baseProject.description ?? undefined,
        videoFilename: baseProject.video_filename,
        videoUrl: baseProject.video_url,
        timelineState: baseProject.timeline_state,
      });
    });

    expect(created).toEqual(baseProject);
    expect(result.current.error).toBeNull();
    expect(result.current.loading).toBe(false);
    expect(fetchMock).toHaveBeenCalledWith(
      expect.stringContaining('/api/projects'),
      expect.objectContaining({ method: 'POST' })
    );
  });

  it('surfaces server errors when project creation fails', async () => {
    const fetchMock = vi.spyOn(global, 'fetch').mockResolvedValue({
      ok: false,
      json: vi.fn().mockResolvedValue({ error: 'validation failed' }),
    } as unknown as Response);

    const { result } = renderHook(() => useProjects());
    let created = null;

    await act(async () => {
      created = await result.current.createProject({
        name: baseProject.name,
        videoFilename: baseProject.video_filename,
        videoUrl: baseProject.video_url,
      });
    });

    expect(created).toBeNull();
    expect(result.current.error).toBe('validation failed');
    expect(result.current.loading).toBe(false);
    expect(fetchMock).toHaveBeenCalledTimes(1);
  });
});
