/**
 * Audio Performance Profiler - 오디오 성능 프로파일링
 * @version 3.2.4
 */

export interface AudioPerformanceMetrics {
  contextTime: number;
  baseLatency: number;
  outputLatency: number;
  bufferUnderruns: number;
  processingLoad: number;
  averageCallbackTime: number;
}

export interface CallbackTiming {
  timestamp: number;
  duration: number;
}

export class AudioPerformanceProfiler {
  private context: AudioContext;
  private callbackTimings: CallbackTiming[] = [];
  private maxTimings: number = 1000;
  private bufferUnderruns: number = 0;
  private isMonitoring: boolean = false;
  private monitorNode: ScriptProcessorNode | null = null;

  constructor(context: AudioContext) {
    this.context = context;
  }

  /**
   * 실시간 모니터링 시작
   */
  startMonitoring(): void {
    if (this.isMonitoring) return;
    this.isMonitoring = true;

    // Use ScriptProcessorNode for timing measurement
    // In production, use AudioWorklet
    this.monitorNode = this.context.createScriptProcessor(256, 1, 1);
    
    let lastTime = 0;
    this.monitorNode.onaudioprocess = (e) => {
      const now = performance.now();
      if (lastTime > 0) {
        const expectedInterval = (e.inputBuffer.length / this.context.sampleRate) * 1000;
        const actualInterval = now - lastTime;
        
        // Detect underrun (callback took too long)
        if (actualInterval > expectedInterval * 1.5) {
          this.bufferUnderruns++;
        }

        this.recordTiming(actualInterval);
      }
      lastTime = now;

      // Pass through
      const input = e.inputBuffer.getChannelData(0);
      const output = e.outputBuffer.getChannelData(0);
      output.set(input);
    };

    // Connect to destination (silent monitoring)
    const silentGain = this.context.createGain();
    silentGain.gain.value = 0;
    this.monitorNode.connect(silentGain);
    silentGain.connect(this.context.destination);
  }

  /**
   * 모니터링 중지
   */
  stopMonitoring(): void {
    if (!this.isMonitoring) return;
    this.isMonitoring = false;

    if (this.monitorNode) {
      this.monitorNode.disconnect();
      this.monitorNode = null;
    }
  }

  private recordTiming(duration: number): void {
    this.callbackTimings.push({
      timestamp: performance.now(),
      duration,
    });

    if (this.callbackTimings.length > this.maxTimings) {
      this.callbackTimings.shift();
    }
  }

  getMetrics(): AudioPerformanceMetrics {
    const avgCallbackTime = this.calculateAverageCallbackTime();
    const processingLoad = this.estimateProcessingLoad();

    return {
      contextTime: this.context.currentTime,
      baseLatency: this.context.baseLatency,
      outputLatency: (this.context as any).outputLatency ?? 0,
      bufferUnderruns: this.bufferUnderruns,
      processingLoad,
      averageCallbackTime: avgCallbackTime,
    };
  }

  private calculateAverageCallbackTime(): number {
    if (this.callbackTimings.length === 0) return 0;
    
    const sum = this.callbackTimings.reduce((acc, t) => acc + t.duration, 0);
    return sum / this.callbackTimings.length;
  }

  private estimateProcessingLoad(): number {
    // Estimate based on timing variance
    if (this.callbackTimings.length < 10) return 0;

    const recent = this.callbackTimings.slice(-100);
    const durations = recent.map(t => t.duration);
    const avg = durations.reduce((a, b) => a + b, 0) / durations.length;
    const max = Math.max(...durations);

    // Higher variance = higher load
    return Math.min(1, (max - avg) / avg);
  }

  getLatencyInfo(): { input: number; output: number; total: number } {
    const base = this.context.baseLatency;
    const output = (this.context as any).outputLatency ?? 0;
    
    return {
      input: base,
      output,
      total: base + output,
    };
  }

  getTimingHistory(): CallbackTiming[] {
    return [...this.callbackTimings];
  }

  resetStats(): void {
    this.callbackTimings = [];
    this.bufferUnderruns = 0;
  }

  logMetrics(): void {
    const metrics = this.getMetrics();
    const latency = this.getLatencyInfo();

    console.group('Audio Performance Metrics');
    console.log(`Context Time: ${metrics.contextTime.toFixed(3)}s`);
    console.log(`Base Latency: ${(latency.input * 1000).toFixed(2)}ms`);
    console.log(`Output Latency: ${(latency.output * 1000).toFixed(2)}ms`);
    console.log(`Total Latency: ${(latency.total * 1000).toFixed(2)}ms`);
    console.log(`Buffer Underruns: ${metrics.bufferUnderruns}`);
    console.log(`Avg Callback Time: ${metrics.averageCallbackTime.toFixed(2)}ms`);
    console.log(`Processing Load: ${(metrics.processingLoad * 100).toFixed(1)}%`);
    console.groupEnd();
  }

  dispose(): void {
    this.stopMonitoring();
    this.callbackTimings = [];
  }
}
