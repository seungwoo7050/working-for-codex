import { jest } from '@jest/globals';
import { MetricsService } from '../services/metrics.service.js';

describe('MetricsService', () => {
  let service: MetricsService;

  beforeEach(() => {
    jest.useFakeTimers();
    service = new MetricsService(false, false);
  });

  afterEach(() => {
    jest.runOnlyPendingTimers();
    jest.clearAllTimers();
    jest.useRealTimers();
    service.shutdown();
  });

  it('records thumbnail extraction metrics and cache ratio', async () => {
    service.recordThumbnailExtraction(0.5, true);
    service.updateThumbnailCacheStats(3, 1);

    const requestMetrics = await service.thumbnailRequests.get();
    expect(requestMetrics.values.some((value) => value.value === 1)).toBe(true);

    const durationMetrics = await service.thumbnailDuration.get();
    const durationSum = durationMetrics.values.find(
      (value) => value.metricName === 'video_editor_thumbnail_duration_seconds_sum'
    );
    expect(durationSum?.value).toBeCloseTo(0.5);

    const cacheMetrics = await service.thumbnailCacheHitRatio.get();
    expect(cacheMetrics.values[0].value).toBeCloseTo(0.75);
  });

  it('increments error counters for different domains', async () => {
    service.recordError('thumbnail', 'ThumbnailError');
    service.recordError('metadata', 'MetadataError');
    service.recordError('api', 'BadRequest', { method: 'GET', route: '/test' });
    service.recordError('ffmpeg', 'CodecError', { operation: 'encode' });

    expect((await service.thumbnailErrors.get()).values[0].value).toBe(1);
    expect((await service.metadataErrors.get()).values[0].value).toBe(1);
    expect((await service.apiErrors.get()).values[0].value).toBe(1);
    expect((await service.ffmpegErrors.get()).values[0].value).toBe(1);
  });
});
