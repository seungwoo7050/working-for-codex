/**
 * Audio Loader - 오디오 파일 로딩
 * @version 3.2.0
 */

export interface LoadProgress {
  loaded: number;
  total: number;
  percent: number;
}

export class AudioLoader {
  private context: AudioContext;
  private cache: Map<string, AudioBuffer> = new Map();

  constructor(context: AudioContext) {
    this.context = context;
  }

  async load(
    url: string,
    onProgress?: (progress: LoadProgress) => void
  ): Promise<AudioBuffer> {
    // Check cache
    const cached = this.cache.get(url);
    if (cached) {
      return cached;
    }

    const response = await fetch(url);
    
    if (!response.ok) {
      throw new Error(`Failed to load audio: ${response.statusText}`);
    }

    const contentLength = response.headers.get('content-length');
    const total = contentLength ? parseInt(contentLength, 10) : 0;
    
    if (onProgress && total > 0 && response.body) {
      // Stream with progress
      const reader = response.body.getReader();
      const chunks: Uint8Array[] = [];
      let loaded = 0;

      while (true) {
        const { done, value } = await reader.read();
        if (done) break;
        
        chunks.push(value);
        loaded += value.length;
        
        onProgress({
          loaded,
          total,
          percent: (loaded / total) * 100,
        });
      }

      const arrayBuffer = this.concatArrayBuffers(chunks);
      const audioBuffer = await this.context.decodeAudioData(arrayBuffer);
      
      this.cache.set(url, audioBuffer);
      return audioBuffer;
    } else {
      // Simple load
      const arrayBuffer = await response.arrayBuffer();
      const audioBuffer = await this.context.decodeAudioData(arrayBuffer);
      
      this.cache.set(url, audioBuffer);
      return audioBuffer;
    }
  }

  async loadFromFile(file: File): Promise<AudioBuffer> {
    const arrayBuffer = await file.arrayBuffer();
    return this.context.decodeAudioData(arrayBuffer);
  }

  async loadFromArrayBuffer(data: ArrayBuffer): Promise<AudioBuffer> {
    return this.context.decodeAudioData(data);
  }

  private concatArrayBuffers(chunks: Uint8Array[]): ArrayBuffer {
    const totalLength = chunks.reduce((acc, chunk) => acc + chunk.length, 0);
    const result = new Uint8Array(totalLength);
    
    let offset = 0;
    for (const chunk of chunks) {
      result.set(chunk, offset);
      offset += chunk.length;
    }
    
    return result.buffer;
  }

  getCached(url: string): AudioBuffer | undefined {
    return this.cache.get(url);
  }

  isCached(url: string): boolean {
    return this.cache.has(url);
  }

  clearCache(): void {
    this.cache.clear();
  }

  removeFromCache(url: string): boolean {
    return this.cache.delete(url);
  }

  getCacheSize(): number {
    return this.cache.size;
  }

  getCacheMemory(): number {
    let total = 0;
    for (const buffer of this.cache.values()) {
      total += buffer.length * buffer.numberOfChannels * 4; // Float32
    }
    return total;
  }
}
