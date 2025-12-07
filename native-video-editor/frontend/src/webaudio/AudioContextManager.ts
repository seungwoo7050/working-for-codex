/**
 * Audio Context Manager - AudioContext 관리
 * @version 3.2.0
 */

export interface AudioContextOptions {
  sampleRate?: number;
  latencyHint?: 'interactive' | 'balanced' | 'playback';
}

export class AudioContextManager {
  private context: AudioContext | null = null;
  private isResumed: boolean = false;
  private stateChangeListeners: Set<(state: AudioContextState) => void> = new Set();

  constructor(private options: AudioContextOptions = {}) {}

  async getContext(): Promise<AudioContext> {
    if (!this.context) {
      this.context = new AudioContext({
        sampleRate: this.options.sampleRate,
        latencyHint: this.options.latencyHint ?? 'interactive',
      });
      
      this.context.onstatechange = () => {
        this.notifyStateChange();
      };
    }

    // Auto-resume on first interaction
    if (this.context.state === 'suspended' && !this.isResumed) {
      await this.resume();
    }

    return this.context;
  }

  async resume(): Promise<void> {
    if (this.context && this.context.state === 'suspended') {
      await this.context.resume();
      this.isResumed = true;
    }
  }

  async suspend(): Promise<void> {
    if (this.context && this.context.state === 'running') {
      await this.context.suspend();
    }
  }

  async close(): Promise<void> {
    if (this.context) {
      await this.context.close();
      this.context = null;
      this.isResumed = false;
    }
  }

  getState(): AudioContextState | null {
    return this.context?.state ?? null;
  }

  getSampleRate(): number {
    return this.context?.sampleRate ?? this.options.sampleRate ?? 44100;
  }

  getCurrentTime(): number {
    return this.context?.currentTime ?? 0;
  }

  getDestination(): AudioDestinationNode | null {
    return this.context?.destination ?? null;
  }

  onStateChange(callback: (state: AudioContextState) => void): () => void {
    this.stateChangeListeners.add(callback);
    return () => this.stateChangeListeners.delete(callback);
  }

  private notifyStateChange(): void {
    const state = this.context?.state;
    if (state) {
      this.stateChangeListeners.forEach(cb => cb(state));
    }
  }

  isAvailable(): boolean {
    return typeof AudioContext !== 'undefined';
  }

  getBaseLatency(): number {
    return this.context?.baseLatency ?? 0;
  }

  getOutputLatency(): number {
    const ctx = this.context;

    if (ctx && 'outputLatency' in ctx) {
      const extendedContext = ctx as AudioContext & { outputLatency?: number };
      return extendedContext.outputLatency ?? 0;
    }

    return 0;
  }
}
