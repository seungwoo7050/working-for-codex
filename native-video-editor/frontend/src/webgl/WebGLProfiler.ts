/**
 * WebGL Profiler - 성능 프로파일링
 * @version 3.1.4
 */

export interface FrameStats {
  frameTime: number;
  drawCalls: number;
  triangles: number;
  stateChanges: number;
  textureBinds: number;
  shaderSwitches: number;
}

export interface ProfilerReport {
  averageFrameTime: number;
  fps: number;
  minFrameTime: number;
  maxFrameTime: number;
  totalFrames: number;
  totalDrawCalls: number;
  averageDrawCalls: number;
  gpuTimings: Array<{ label: string; time: number }>;
}

export class WebGLProfiler {
  private gl: WebGL2RenderingContext;
  private ext: {
    timer: any | null;
  };
  
  private frameHistory: FrameStats[] = [];
  private maxHistorySize: number = 120;
  
  private currentFrame: FrameStats = this.createEmptyStats();
  private frameStartTime: number = 0;
  private isEnabled: boolean = true;
  
  private gpuQueries: Map<string, WebGLQuery> = new Map();
  private gpuTimings: Map<string, number> = new Map();

  constructor(gl: WebGL2RenderingContext) {
    this.gl = gl;
    
    this.ext = {
      timer: gl.getExtension('EXT_disjoint_timer_query_webgl2'),
    };
  }

  private createEmptyStats(): FrameStats {
    return {
      frameTime: 0,
      drawCalls: 0,
      triangles: 0,
      stateChanges: 0,
      textureBinds: 0,
      shaderSwitches: 0,
    };
  }

  enable(): void {
    this.isEnabled = true;
  }

  disable(): void {
    this.isEnabled = false;
  }

  beginFrame(): void {
    if (!this.isEnabled) return;
    
    this.frameStartTime = performance.now();
    this.currentFrame = this.createEmptyStats();
  }

  endFrame(): void {
    if (!this.isEnabled) return;
    
    this.currentFrame.frameTime = performance.now() - this.frameStartTime;
    
    this.frameHistory.push({ ...this.currentFrame });
    if (this.frameHistory.length > this.maxHistorySize) {
      this.frameHistory.shift();
    }
    
    // Collect GPU timing results
    this.collectGPUTimings();
  }

  recordDrawCall(triangles: number = 0): void {
    if (!this.isEnabled) return;
    this.currentFrame.drawCalls++;
    this.currentFrame.triangles += triangles;
  }

  recordStateChange(): void {
    if (!this.isEnabled) return;
    this.currentFrame.stateChanges++;
  }

  recordTextureBind(): void {
    if (!this.isEnabled) return;
    this.currentFrame.textureBinds++;
  }

  recordShaderSwitch(): void {
    if (!this.isEnabled) return;
    this.currentFrame.shaderSwitches++;
  }

  beginGPUTiming(label: string): void {
    if (!this.isEnabled || !this.ext.timer) return;
    
    const query = this.gl.createQuery();
    if (!query) return;
    
    this.gl.beginQuery(this.ext.timer.TIME_ELAPSED_EXT, query);
    this.gpuQueries.set(label, query);
  }

  endGPUTiming(label: string): void {
    if (!this.isEnabled || !this.ext.timer) return;
    
    if (this.gpuQueries.has(label)) {
      this.gl.endQuery(this.ext.timer.TIME_ELAPSED_EXT);
    }
  }

  private collectGPUTimings(): void {
    if (!this.ext.timer) return;
    
    for (const [label, query] of this.gpuQueries.entries()) {
      const available = this.gl.getQueryParameter(query, this.gl.QUERY_RESULT_AVAILABLE);
      const disjoint = this.gl.getParameter(this.ext.timer.GPU_DISJOINT_EXT);
      
      if (available && !disjoint) {
        const elapsed = this.gl.getQueryParameter(query, this.gl.QUERY_RESULT);
        this.gpuTimings.set(label, elapsed / 1000000); // ns to ms
        this.gl.deleteQuery(query);
        this.gpuQueries.delete(label);
      }
    }
  }

  getCurrentFrameStats(): FrameStats {
    return { ...this.currentFrame };
  }

  getFrameHistory(): FrameStats[] {
    return [...this.frameHistory];
  }

  getReport(): ProfilerReport {
    if (this.frameHistory.length === 0) {
      return {
        averageFrameTime: 0,
        fps: 0,
        minFrameTime: 0,
        maxFrameTime: 0,
        totalFrames: 0,
        totalDrawCalls: 0,
        averageDrawCalls: 0,
        gpuTimings: [],
      };
    }
    
    const frameTimes = this.frameHistory.map(f => f.frameTime);
    const totalFrameTime = frameTimes.reduce((a, b) => a + b, 0);
    const totalDrawCalls = this.frameHistory.reduce((a, f) => a + f.drawCalls, 0);
    
    return {
      averageFrameTime: totalFrameTime / this.frameHistory.length,
      fps: 1000 / (totalFrameTime / this.frameHistory.length),
      minFrameTime: Math.min(...frameTimes),
      maxFrameTime: Math.max(...frameTimes),
      totalFrames: this.frameHistory.length,
      totalDrawCalls,
      averageDrawCalls: totalDrawCalls / this.frameHistory.length,
      gpuTimings: Array.from(this.gpuTimings.entries()).map(([label, time]) => ({
        label,
        time,
      })),
    };
  }

  getLastNFrames(n: number): FrameStats[] {
    return this.frameHistory.slice(-n);
  }

  getAverageFPS(): number {
    if (this.frameHistory.length === 0) return 0;
    
    const totalTime = this.frameHistory.reduce((a, f) => a + f.frameTime, 0);
    return 1000 / (totalTime / this.frameHistory.length);
  }

  reset(): void {
    this.frameHistory = [];
    this.gpuTimings.clear();
    
    for (const query of this.gpuQueries.values()) {
      this.gl.deleteQuery(query);
    }
    this.gpuQueries.clear();
  }

  logReport(): void {
    const report = this.getReport();
    console.group('WebGL Profiler Report');
    console.log(`FPS: ${report.fps.toFixed(1)}`);
    console.log(`Avg Frame Time: ${report.averageFrameTime.toFixed(2)}ms`);
    console.log(`Min/Max: ${report.minFrameTime.toFixed(2)}ms / ${report.maxFrameTime.toFixed(2)}ms`);
    console.log(`Draw Calls (avg): ${report.averageDrawCalls.toFixed(1)}`);
    
    if (report.gpuTimings.length > 0) {
      console.group('GPU Timings');
      for (const { label, time } of report.gpuTimings) {
        console.log(`${label}: ${time.toFixed(2)}ms`);
      }
      console.groupEnd();
    }
    
    console.groupEnd();
  }

  dispose(): void {
    this.reset();
  }
}
