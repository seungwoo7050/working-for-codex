/**
 * Audio Memory Manager - 오디오 메모리 관리
 * @version 3.2.4
 */

export interface AudioMemoryStats {
  estimatedBufferMemory: number;
  activeNodes: number;
  cachedBuffers: number;
  peakMemory: number;
}

export class AudioMemoryManager {
  private trackedBuffers: Map<string, { buffer: AudioBuffer; size: number }> = new Map();
  private activeNodeCount: number = 0;
  private peakMemory: number = 0;
  private memoryWarningThreshold: number;
  private onMemoryWarning?: (usage: number) => void;

  constructor(
    memoryWarningThresholdMB: number = 256,
    onMemoryWarning?: (usage: number) => void
  ) {
    this.memoryWarningThreshold = memoryWarningThresholdMB * 1024 * 1024;
    this.onMemoryWarning = onMemoryWarning;
  }

  trackBuffer(id: string, buffer: AudioBuffer): void {
    const size = this.calculateBufferSize(buffer);
    this.trackedBuffers.set(id, { buffer, size });
    this.checkMemoryUsage();
  }

  untrackBuffer(id: string): void {
    this.trackedBuffers.delete(id);
  }

  trackNode(): void {
    this.activeNodeCount++;
  }

  untrackNode(): void {
    this.activeNodeCount = Math.max(0, this.activeNodeCount - 1);
  }

  private calculateBufferSize(buffer: AudioBuffer): number {
    // Each sample is a Float32 (4 bytes)
    return buffer.length * buffer.numberOfChannels * 4;
  }

  private checkMemoryUsage(): void {
    const usage = this.getEstimatedMemory();
    
    if (usage > this.peakMemory) {
      this.peakMemory = usage;
    }

    if (usage > this.memoryWarningThreshold && this.onMemoryWarning) {
      this.onMemoryWarning(usage);
    }
  }

  getEstimatedMemory(): number {
    let total = 0;
    for (const { size } of this.trackedBuffers.values()) {
      total += size;
    }
    return total;
  }

  getStats(): AudioMemoryStats {
    return {
      estimatedBufferMemory: this.getEstimatedMemory(),
      activeNodes: this.activeNodeCount,
      cachedBuffers: this.trackedBuffers.size,
      peakMemory: this.peakMemory,
    };
  }

  /**
   * 사용되지 않는 버퍼 정리 (수동 호출)
   */
  cleanup(keepIds: Set<string>): number {
    let freed = 0;
    
    for (const [id, { size }] of this.trackedBuffers.entries()) {
      if (!keepIds.has(id)) {
        this.trackedBuffers.delete(id);
        freed += size;
      }
    }

    return freed;
  }

  /**
   * LRU 기반 메모리 해제 (특정 크기까지)
   */
  evictToSize(targetSizeBytes: number): string[] {
    const evicted: string[] = [];
    const current = this.getEstimatedMemory();
    
    if (current <= targetSizeBytes) {
      return evicted;
    }

    // Simple eviction: remove oldest first
    // In production, you'd track access times
    const entries = Array.from(this.trackedBuffers.entries());
    let freedMemory = 0;
    const neededToFree = current - targetSizeBytes;

    for (const [id, { size }] of entries) {
      if (freedMemory >= neededToFree) break;
      
      this.trackedBuffers.delete(id);
      freedMemory += size;
      evicted.push(id);
    }

    return evicted;
  }

  setWarningThreshold(thresholdMB: number): void {
    this.memoryWarningThreshold = thresholdMB * 1024 * 1024;
  }

  setWarningCallback(callback: (usage: number) => void): void {
    this.onMemoryWarning = callback;
  }

  clear(): void {
    this.trackedBuffers.clear();
    this.activeNodeCount = 0;
  }

  formatBytes(bytes: number): string {
    if (bytes < 1024) return `${bytes} B`;
    if (bytes < 1024 * 1024) return `${(bytes / 1024).toFixed(2)} KB`;
    return `${(bytes / (1024 * 1024)).toFixed(2)} MB`;
  }

  logStats(): void {
    const stats = this.getStats();
    console.group('Audio Memory Stats');
    console.log(`Buffer Memory: ${this.formatBytes(stats.estimatedBufferMemory)}`);
    console.log(`Peak Memory: ${this.formatBytes(stats.peakMemory)}`);
    console.log(`Cached Buffers: ${stats.cachedBuffers}`);
    console.log(`Active Nodes: ${stats.activeNodes}`);
    console.groupEnd();
  }
}
