/**
 * GPU Memory Optimizer - GPU 메모리 최적화
 * @version 3.1.4
 */

export interface TextureInfo {
  id: string;
  texture: WebGLTexture;
  width: number;
  height: number;
  format: number;
  lastUsed: number;
  bytesPerPixel: number;
}

export interface MemoryStats {
  textureCount: number;
  textureMemory: number;
  bufferMemory: number;
  totalMemory: number;
  memoryLimit: number;
  usagePercent: number;
}

export class GPUMemoryOptimizer {
  private gl: WebGL2RenderingContext;
  private textures: Map<string, TextureInfo> = new Map();
  private bufferMemory: number = 0;
  private memoryLimit: number;
  private evictionThreshold: number;
  
  private evictionCount: number = 0;
  private peakMemory: number = 0;

  constructor(
    gl: WebGL2RenderingContext,
    memoryLimitMB: number = 256,
    evictionThreshold: number = 0.8
  ) {
    this.gl = gl;
    this.memoryLimit = memoryLimitMB * 1024 * 1024;
    this.evictionThreshold = evictionThreshold;
  }

  registerTexture(
    id: string,
    texture: WebGLTexture,
    width: number,
    height: number,
    format: number = this.gl.RGBA
  ): void {
    const bytesPerPixel = this.getBytesPerPixel(format);
    
    this.textures.set(id, {
      id,
      texture,
      width,
      height,
      format,
      lastUsed: Date.now(),
      bytesPerPixel,
    });
    
    this.checkMemoryPressure();
    this.updatePeakMemory();
  }

  unregisterTexture(id: string): void {
    const info = this.textures.get(id);
    if (info) {
      this.gl.deleteTexture(info.texture);
      this.textures.delete(id);
    }
  }

  registerBuffer(sizeBytes: number): void {
    this.bufferMemory += sizeBytes;
    this.checkMemoryPressure();
    this.updatePeakMemory();
  }

  unregisterBuffer(sizeBytes: number): void {
    this.bufferMemory = Math.max(0, this.bufferMemory - sizeBytes);
  }

  touchTexture(id: string): void {
    const info = this.textures.get(id);
    if (info) {
      info.lastUsed = Date.now();
    }
  }

  private getBytesPerPixel(format: number): number {
    const gl = this.gl;
    
    switch (format) {
      case gl.RGBA:
      case gl.RGBA8:
        return 4;
      case gl.RGB:
      case gl.RGB8:
        return 3;
      case gl.RG:
      case gl.RG8:
        return 2;
      case gl.RED:
      case gl.R8:
        return 1;
      case gl.RGBA16F:
        return 8;
      case gl.RGBA32F:
        return 16;
      default:
        return 4;
    }
  }

  private calculateTextureMemory(): number {
    let total = 0;
    for (const info of this.textures.values()) {
      total += info.width * info.height * info.bytesPerPixel;
    }
    return total;
  }

  private getTotalMemory(): number {
    return this.calculateTextureMemory() + this.bufferMemory;
  }

  private checkMemoryPressure(): void {
    const totalMemory = this.getTotalMemory();
    
    if (totalMemory > this.memoryLimit * this.evictionThreshold) {
      this.evictLRU();
    }
  }

  private evictLRU(): void {
    // Sort textures by last used time
    const sortedTextures = Array.from(this.textures.values())
      .sort((a, b) => a.lastUsed - b.lastUsed);
    
    const targetMemory = this.memoryLimit * 0.6;
    
    for (const info of sortedTextures) {
      if (this.getTotalMemory() <= targetMemory) break;
      
      this.gl.deleteTexture(info.texture);
      this.textures.delete(info.id);
      this.evictionCount++;
    }
  }

  forceEviction(targetUsagePercent: number = 0.5): number {
    const targetMemory = this.memoryLimit * targetUsagePercent;
    let evicted = 0;
    
    const sortedTextures = Array.from(this.textures.values())
      .sort((a, b) => a.lastUsed - b.lastUsed);
    
    for (const info of sortedTextures) {
      if (this.getTotalMemory() <= targetMemory) break;
      
      this.gl.deleteTexture(info.texture);
      this.textures.delete(info.id);
      evicted++;
    }
    
    this.evictionCount += evicted;
    return evicted;
  }

  private updatePeakMemory(): void {
    const current = this.getTotalMemory();
    if (current > this.peakMemory) {
      this.peakMemory = current;
    }
  }

  getStats(): MemoryStats {
    const textureMemory = this.calculateTextureMemory();
    const totalMemory = textureMemory + this.bufferMemory;
    
    return {
      textureCount: this.textures.size,
      textureMemory,
      bufferMemory: this.bufferMemory,
      totalMemory,
      memoryLimit: this.memoryLimit,
      usagePercent: totalMemory / this.memoryLimit,
    };
  }

  getDetailedStats(): {
    stats: MemoryStats;
    evictionCount: number;
    peakMemory: number;
    textureDetails: Array<{ id: string; size: number; age: number }>;
  } {
    const now = Date.now();
    const textureDetails = Array.from(this.textures.values()).map(info => ({
      id: info.id,
      size: info.width * info.height * info.bytesPerPixel,
      age: now - info.lastUsed,
    }));
    
    return {
      stats: this.getStats(),
      evictionCount: this.evictionCount,
      peakMemory: this.peakMemory,
      textureDetails,
    };
  }

  setMemoryLimit(limitMB: number): void {
    this.memoryLimit = limitMB * 1024 * 1024;
    this.checkMemoryPressure();
  }

  clear(): void {
    for (const info of this.textures.values()) {
      this.gl.deleteTexture(info.texture);
    }
    this.textures.clear();
    this.bufferMemory = 0;
  }

  dispose(): void {
    this.clear();
  }
}
