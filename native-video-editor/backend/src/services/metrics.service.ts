/**
 * Prometheus Metrics Service
 * cpp-pvp-server M1.7 pattern - Comprehensive monitoring
 */

import { Registry, Counter, Histogram, Gauge, collectDefaultMetrics } from 'prom-client';

export class MetricsService {
  private registry: Registry;
  private defaultMetricsInterval?: NodeJS.Timeout;
  private systemMetricsInterval?: NodeJS.Timeout;

  // Thumbnail extraction metrics
  public thumbnailDuration: Histogram;
  public thumbnailRequests: Counter;
  public thumbnailCacheHitRatio: Gauge;
  public thumbnailErrors: Counter;

  // Metadata extraction metrics
  public metadataDuration: Histogram;
  public metadataRequests: Counter;
  public metadataErrors: Counter;

  // API metrics
  public apiLatency: Histogram;
  public apiRequests: Counter;
  public apiErrors: Counter;

  // FFmpeg metrics
  public ffmpegDuration: Histogram;
  public ffmpegErrors: Counter;

  // System metrics
  public memoryUsage: Gauge;
  public cpuUsage: Gauge;

  constructor(enableSystemMetrics = true, enableDefaultMetrics = true) {
    this.registry = new Registry();

    // Enable default metrics (CPU, memory, event loop, etc.)
    if (enableDefaultMetrics) {
      this.defaultMetricsInterval = collectDefaultMetrics({
        register: this.registry,
        prefix: 'video_editor_',
      }) as unknown as NodeJS.Timeout | undefined;
    }

    // Thumbnail extraction metrics
    this.thumbnailDuration = new Histogram({
      name: 'video_editor_thumbnail_duration_seconds',
      help: 'Duration of thumbnail extraction operations in seconds',
      labelNames: ['status'],
      buckets: [0.01, 0.025, 0.05, 0.1, 0.25, 0.5, 1, 2.5, 5, 10],
      registers: [this.registry],
    });

    this.thumbnailRequests = new Counter({
      name: 'video_editor_thumbnail_requests_total',
      help: 'Total number of thumbnail extraction requests',
      labelNames: ['status'],
      registers: [this.registry],
    });

    this.thumbnailCacheHitRatio = new Gauge({
      name: 'video_editor_thumbnail_cache_hit_ratio',
      help: 'Cache hit ratio for thumbnail extraction (0-1)',
      registers: [this.registry],
    });

    this.thumbnailErrors = new Counter({
      name: 'video_editor_thumbnail_errors_total',
      help: 'Total number of thumbnail extraction errors',
      labelNames: ['error_type'],
      registers: [this.registry],
    });

    // Metadata extraction metrics
    this.metadataDuration = new Histogram({
      name: 'video_editor_metadata_duration_seconds',
      help: 'Duration of metadata extraction operations in seconds',
      labelNames: ['status'],
      buckets: [0.001, 0.005, 0.01, 0.025, 0.05, 0.1, 0.25, 0.5, 1],
      registers: [this.registry],
    });

    this.metadataRequests = new Counter({
      name: 'video_editor_metadata_requests_total',
      help: 'Total number of metadata extraction requests',
      labelNames: ['status'],
      registers: [this.registry],
    });

    this.metadataErrors = new Counter({
      name: 'video_editor_metadata_errors_total',
      help: 'Total number of metadata extraction errors',
      labelNames: ['error_type'],
      registers: [this.registry],
    });

    // API metrics
    this.apiLatency = new Histogram({
      name: 'video_editor_api_latency_seconds',
      help: 'HTTP API request latency in seconds',
      labelNames: ['method', 'route', 'status_code'],
      buckets: [0.01, 0.05, 0.1, 0.5, 1, 2, 5, 10],
      registers: [this.registry],
    });

    this.apiRequests = new Counter({
      name: 'video_editor_api_requests_total',
      help: 'Total number of API requests',
      labelNames: ['method', 'route', 'status_code'],
      registers: [this.registry],
    });

    this.apiErrors = new Counter({
      name: 'video_editor_api_errors_total',
      help: 'Total number of API errors',
      labelNames: ['method', 'route', 'error_type'],
      registers: [this.registry],
    });

    // FFmpeg metrics
    this.ffmpegDuration = new Histogram({
      name: 'video_editor_ffmpeg_duration_seconds',
      help: 'Duration of FFmpeg operations in seconds',
      labelNames: ['operation', 'status'],
      buckets: [0.1, 0.5, 1, 2, 5, 10, 30, 60, 120],
      registers: [this.registry],
    });

    this.ffmpegErrors = new Counter({
      name: 'video_editor_ffmpeg_errors_total',
      help: 'Total number of FFmpeg errors',
      labelNames: ['operation', 'error_type'],
      registers: [this.registry],
    });

    // System metrics
    this.memoryUsage = new Gauge({
      name: 'video_editor_memory_usage_bytes',
      help: 'Memory usage in bytes',
      labelNames: ['type'],
      registers: [this.registry],
    });

    this.cpuUsage = new Gauge({
      name: 'video_editor_cpu_usage_percent',
      help: 'CPU usage percentage',
      registers: [this.registry],
    });

    // Update system metrics periodically
    if (enableSystemMetrics) {
      this.startSystemMetricsCollection();
    }
  }

  /**
   * Start collecting system metrics
   */
  private startSystemMetricsCollection(): void {
    this.systemMetricsInterval = setInterval(() => {
      const memUsage = process.memoryUsage();

      this.memoryUsage.set({ type: 'rss' }, memUsage.rss);
      this.memoryUsage.set({ type: 'heap_total' }, memUsage.heapTotal);
      this.memoryUsage.set({ type: 'heap_used' }, memUsage.heapUsed);
      this.memoryUsage.set({ type: 'external' }, memUsage.external);

      // CPU usage (simple approximation)
      const cpuUsage = process.cpuUsage();
      const totalUsage = (cpuUsage.user + cpuUsage.system) / 1000000; // Convert to seconds
      this.cpuUsage.set(totalUsage);
    }, 5000); // Every 5 seconds
  }

  /**
   * Get metrics in Prometheus format
   */
  public async getMetrics(): Promise<string> {
    return this.registry.metrics();
  }

  /**
   * Get registry
   */
  public getRegistry(): Registry {
    return this.registry;
  }

  /**
   * Stop background metric collectors to avoid open handles
   */
  public shutdown(): void {
    if (this.defaultMetricsInterval) {
      clearInterval(this.defaultMetricsInterval);
      this.defaultMetricsInterval = undefined;
    }

    if (this.systemMetricsInterval) {
      clearInterval(this.systemMetricsInterval);
      this.systemMetricsInterval = undefined;
    }

    this.registry.clear();
  }

  /**
   * Record thumbnail extraction
   */
  public recordThumbnailExtraction(durationSeconds: number, success: boolean): void {
    const status = success ? 'success' : 'error';
    this.thumbnailDuration.observe({ status }, durationSeconds);
    this.thumbnailRequests.inc({ status });
  }

  /**
   * Record thumbnail cache stats
   */
  public updateThumbnailCacheStats(hits: number, misses: number): void {
    const total = hits + misses;
    const hitRatio = total > 0 ? hits / total : 0;
    this.thumbnailCacheHitRatio.set(hitRatio);
  }

  /**
   * Record metadata extraction
   */
  public recordMetadataExtraction(durationSeconds: number, success: boolean): void {
    const status = success ? 'success' : 'error';
    this.metadataDuration.observe({ status }, durationSeconds);
    this.metadataRequests.inc({ status });
  }

  /**
   * Record API request
   */
  public recordApiRequest(
    method: string,
    route: string,
    statusCode: number,
    durationSeconds: number
  ): void {
    this.apiLatency.observe({ method, route, status_code: statusCode.toString() }, durationSeconds);
    this.apiRequests.inc({ method, route, status_code: statusCode.toString() });
  }

  /**
   * Record FFmpeg operation
   */
  public recordFFmpegOperation(
    operation: string,
    durationSeconds: number,
    success: boolean
  ): void {
    const status = success ? 'success' : 'error';
    this.ffmpegDuration.observe({ operation, status }, durationSeconds);
  }

  /**
   * Record error
   */
  public recordError(type: 'thumbnail' | 'metadata' | 'api' | 'ffmpeg', errorType: string, labels?: Record<string, string>): void {
    switch (type) {
      case 'thumbnail':
        this.thumbnailErrors.inc({ error_type: errorType });
        break;
      case 'metadata':
        this.metadataErrors.inc({ error_type: errorType });
        break;
      case 'api':
        this.apiErrors.inc({ ...labels, error_type: errorType });
        break;
      case 'ffmpeg':
        this.ffmpegErrors.inc({ ...labels, error_type: errorType });
        break;
    }
  }
}

// Singleton instance (disable background collectors during tests)
const isTestEnv = process.env.NODE_ENV === 'test';
export const metricsService = new MetricsService(!isTestEnv, !isTestEnv);
