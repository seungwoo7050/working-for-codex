/**
 * Audio Debug - 디버깅 유틸리티
 * @version 3.2.0
 */

export interface AudioDebugInfo {
  contextState: AudioContextState | null;
  sampleRate: number;
  currentTime: number;
  baseLatency: number;
  outputLatency: number;
  activeNodes: number;
}

export class AudioDebug {
  private context: AudioContext;
  private nodeCount: number = 0;
  private logs: Array<{ timestamp: number; message: string; level: string }> = [];
  private maxLogs: number = 100;
  private isEnabled: boolean = true;

  constructor(context: AudioContext) {
    this.context = context;
  }

  enable(): void {
    this.isEnabled = true;
  }

  disable(): void {
    this.isEnabled = false;
  }

  log(message: string): void {
    if (!this.isEnabled) return;
    this.addLog('info', message);
    console.log(`[AudioDebug] ${message}`);
  }

  warn(message: string): void {
    if (!this.isEnabled) return;
    this.addLog('warn', message);
    console.warn(`[AudioDebug] ${message}`);
  }

  error(message: string): void {
    if (!this.isEnabled) return;
    this.addLog('error', message);
    console.error(`[AudioDebug] ${message}`);
  }

  private addLog(level: string, message: string): void {
    this.logs.push({
      timestamp: Date.now(),
      level,
      message,
    });

    if (this.logs.length > this.maxLogs) {
      this.logs.shift();
    }
  }

  trackNode(node: AudioNode): void {
    this.nodeCount++;
    this.log(`Node created: ${node.constructor.name} (total: ${this.nodeCount})`);
  }

  untrackNode(node: AudioNode): void {
    this.nodeCount--;
    this.log(`Node disposed: ${node.constructor.name} (total: ${this.nodeCount})`);
  }

  getInfo(): AudioDebugInfo {
    const ctx = this.context as AudioContext & { outputLatency?: number };
    return {
      contextState: this.context.state,
      sampleRate: this.context.sampleRate,
      currentTime: this.context.currentTime,
      baseLatency: this.context.baseLatency,
      outputLatency: ctx.outputLatency ?? 0,
      activeNodes: this.nodeCount,
    };
  }

  getLogs(): Array<{ timestamp: number; message: string; level: string }> {
    return [...this.logs];
  }

  clearLogs(): void {
    this.logs = [];
  }

  logInfo(): void {
    const info = this.getInfo();
    console.group('Audio Debug Info');
    console.log(`State: ${info.contextState}`);
    console.log(`Sample Rate: ${info.sampleRate} Hz`);
    console.log(`Current Time: ${info.currentTime.toFixed(3)} s`);
    console.log(`Base Latency: ${(info.baseLatency * 1000).toFixed(2)} ms`);
    console.log(`Output Latency: ${(info.outputLatency * 1000).toFixed(2)} ms`);
    console.log(`Active Nodes: ${info.activeNodes}`);
    console.groupEnd();
  }

  validateNode(node: AudioNode): boolean {
    if (!node.context) {
      this.error('Node has no context');
      return false;
    }

    if (node.context !== this.context) {
      this.warn('Node belongs to different context');
    }

    return true;
  }

  validateBuffer(buffer: AudioBuffer): boolean {
    if (buffer.length === 0) {
      this.error('Buffer is empty');
      return false;
    }

    if (buffer.sampleRate !== this.context.sampleRate) {
      this.warn(`Buffer sample rate (${buffer.sampleRate}) differs from context (${this.context.sampleRate})`);
    }

    return true;
  }

  getNodeCount(): number {
    return this.nodeCount;
  }
}
