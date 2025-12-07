/**
 * Audio Buffer Manager - 버퍼 관리
 * @version 3.2.0
 */

export interface BufferInfo {
  id: string;
  buffer: AudioBuffer;
  duration: number;
  sampleRate: number;
  channels: number;
  lastUsed: number;
}

export class AudioBufferManager {
  private context: AudioContext;
  private buffers: Map<string, BufferInfo> = new Map();
  private memoryLimit: number;

  constructor(context: AudioContext, memoryLimitMB: number = 128) {
    this.context = context;
    this.memoryLimit = memoryLimitMB * 1024 * 1024;
  }

  add(id: string, buffer: AudioBuffer): void {
    this.buffers.set(id, {
      id,
      buffer,
      duration: buffer.duration,
      sampleRate: buffer.sampleRate,
      channels: buffer.numberOfChannels,
      lastUsed: Date.now(),
    });

    this.checkMemoryLimit();
  }

  get(id: string): AudioBuffer | null {
    const info = this.buffers.get(id);
    if (info) {
      info.lastUsed = Date.now();
      return info.buffer;
    }
    return null;
  }

  has(id: string): boolean {
    return this.buffers.has(id);
  }

  remove(id: string): boolean {
    return this.buffers.delete(id);
  }

  createEmpty(
    channels: number,
    length: number,
    sampleRate?: number
  ): AudioBuffer {
    return this.context.createBuffer(
      channels,
      length,
      sampleRate ?? this.context.sampleRate
    );
  }

  createFromChannelData(
    channelData: Float32Array[],
    sampleRate?: number
  ): AudioBuffer {
    const buffer = this.context.createBuffer(
      channelData.length,
      channelData[0].length,
      sampleRate ?? this.context.sampleRate
    );

    for (let i = 0; i < channelData.length; i++) {
      buffer.copyToChannel(new Float32Array(channelData[i]), i);
    }

    return buffer;
  }

  clone(buffer: AudioBuffer): AudioBuffer {
    const newBuffer = this.createEmpty(
      buffer.numberOfChannels,
      buffer.length,
      buffer.sampleRate
    );

    for (let i = 0; i < buffer.numberOfChannels; i++) {
      const channelData = new Float32Array(buffer.length);
      buffer.copyFromChannel(channelData, i);
      newBuffer.copyToChannel(channelData, i);
    }

    return newBuffer;
  }

  slice(
    buffer: AudioBuffer,
    startSample: number,
    endSample: number
  ): AudioBuffer {
    const length = endSample - startSample;
    const newBuffer = this.createEmpty(
      buffer.numberOfChannels,
      length,
      buffer.sampleRate
    );

    for (let i = 0; i < buffer.numberOfChannels; i++) {
      const channelData = new Float32Array(length);
      buffer.copyFromChannel(channelData, i, startSample);
      newBuffer.copyToChannel(channelData, i);
    }

    return newBuffer;
  }

  concat(buffers: AudioBuffer[]): AudioBuffer {
    if (buffers.length === 0) {
      throw new Error('No buffers to concat');
    }

    const totalLength = buffers.reduce((acc, b) => acc + b.length, 0);
    const channels = Math.max(...buffers.map(b => b.numberOfChannels));
    const sampleRate = buffers[0].sampleRate;

    const result = this.createEmpty(channels, totalLength, sampleRate);

    let offset = 0;
    for (const buffer of buffers) {
      for (let i = 0; i < buffer.numberOfChannels; i++) {
        const channelData = new Float32Array(buffer.length);
        buffer.copyFromChannel(channelData, i);
        result.copyToChannel(channelData, i, offset);
      }
      offset += buffer.length;
    }

    return result;
  }

  private getBufferMemory(buffer: AudioBuffer): number {
    return buffer.length * buffer.numberOfChannels * 4;
  }

  private getTotalMemory(): number {
    let total = 0;
    for (const info of this.buffers.values()) {
      total += this.getBufferMemory(info.buffer);
    }
    return total;
  }

  private checkMemoryLimit(): void {
    if (this.getTotalMemory() <= this.memoryLimit) return;

    // Evict LRU
    const sorted = Array.from(this.buffers.values())
      .sort((a, b) => a.lastUsed - b.lastUsed);

    while (this.getTotalMemory() > this.memoryLimit * 0.8 && sorted.length > 0) {
      const toRemove = sorted.shift();
      if (toRemove) {
        this.buffers.delete(toRemove.id);
      }
    }
  }

  getStats(): {
    count: number;
    totalMemory: number;
    memoryLimit: number;
  } {
    return {
      count: this.buffers.size,
      totalMemory: this.getTotalMemory(),
      memoryLimit: this.memoryLimit,
    };
  }

  clear(): void {
    this.buffers.clear();
  }
}
