/**
 * Audio Buffer Pool - 오디오 버퍼 풀링
 * @version 3.2.4
 */

export interface PooledBuffer {
  buffer: AudioBuffer;
  inUse: boolean;
  lastUsed: number;
}

export class AudioBufferPool {
  private context: AudioContext;
  private pools: Map<string, PooledBuffer[]> = new Map();
  private maxPoolSize: number;
  private cleanupInterval: number | null = null;

  constructor(context: AudioContext, maxPoolSize: number = 20) {
    this.context = context;
    this.maxPoolSize = maxPoolSize;
    
    // Periodic cleanup
    this.cleanupInterval = window.setInterval(() => {
      this.cleanup();
    }, 30000);
  }

  private getKey(channels: number, length: number, sampleRate: number): string {
    return `${channels}:${length}:${sampleRate}`;
  }

  acquire(channels: number, length: number, sampleRate?: number): AudioBuffer {
    const sr = sampleRate ?? this.context.sampleRate;
    const key = this.getKey(channels, length, sr);
    
    const pool = this.pools.get(key);
    if (pool) {
      const available = pool.find(pb => !pb.inUse);
      if (available) {
        available.inUse = true;
        available.lastUsed = Date.now();
        
        // Clear buffer data
        for (let i = 0; i < available.buffer.numberOfChannels; i++) {
          const data = available.buffer.getChannelData(i);
          data.fill(0);
        }
        
        return available.buffer;
      }
    }

    // Create new buffer
    const buffer = this.context.createBuffer(channels, length, sr);
    this.addToPool(key, buffer, true);
    
    return buffer;
  }

  release(buffer: AudioBuffer): void {
    const key = this.getKey(
      buffer.numberOfChannels,
      buffer.length,
      buffer.sampleRate
    );
    
    const pool = this.pools.get(key);
    if (pool) {
      const pooled = pool.find(pb => pb.buffer === buffer);
      if (pooled) {
        pooled.inUse = false;
        pooled.lastUsed = Date.now();
      }
    }
  }

  private addToPool(key: string, buffer: AudioBuffer, inUse: boolean): void {
    let pool = this.pools.get(key);
    if (!pool) {
      pool = [];
      this.pools.set(key, pool);
    }

    if (pool.length < this.maxPoolSize) {
      pool.push({
        buffer,
        inUse,
        lastUsed: Date.now(),
      });
    }
  }

  private cleanup(): void {
    const now = Date.now();
    const maxAge = 60000; // 1 minute

    for (const [key, pool] of this.pools.entries()) {
      // Remove old unused buffers
      const remaining = pool.filter(pb => {
        if (pb.inUse) return true;
        return now - pb.lastUsed < maxAge;
      });

      if (remaining.length === 0) {
        this.pools.delete(key);
      } else {
        this.pools.set(key, remaining);
      }
    }
  }

  getStats(): {
    poolCount: number;
    totalBuffers: number;
    inUse: number;
    available: number;
  } {
    let totalBuffers = 0;
    let inUse = 0;

    for (const pool of this.pools.values()) {
      totalBuffers += pool.length;
      inUse += pool.filter(pb => pb.inUse).length;
    }

    return {
      poolCount: this.pools.size,
      totalBuffers,
      inUse,
      available: totalBuffers - inUse,
    };
  }

  clear(): void {
    this.pools.clear();
  }

  dispose(): void {
    if (this.cleanupInterval !== null) {
      clearInterval(this.cleanupInterval);
      this.cleanupInterval = null;
    }
    this.clear();
  }
}
