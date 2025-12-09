/**
 * Shader Cache - 컴파일된 셰이더 캐싱
 * @version 3.1.1
 */

import { ShaderProgram, ShaderProgramOptions } from './ShaderProgram';

export class ShaderCache {
  private gl: WebGL2RenderingContext;
  private cache: Map<string, ShaderProgram> = new Map();

  constructor(gl: WebGL2RenderingContext) {
    this.gl = gl;
  }

  private generateKey(vertexSource: string, fragmentSource: string): string {
    // Simple hash using string length and first/last characters
    const vHash = this.simpleHash(vertexSource);
    const fHash = this.simpleHash(fragmentSource);
    return `${vHash}_${fHash}`;
  }

  private simpleHash(str: string): string {
    let hash = 0;
    for (let i = 0; i < str.length; i++) {
      const char = str.charCodeAt(i);
      hash = ((hash << 5) - hash) + char;
      hash = hash & hash; // Convert to 32bit integer
    }
    return hash.toString(36);
  }

  get(vertexSource: string, fragmentSource: string): ShaderProgram | null {
    const key = this.generateKey(vertexSource, fragmentSource);
    return this.cache.get(key) || null;
  }

  getOrCreate(options: ShaderProgramOptions): ShaderProgram {
    const key = this.generateKey(options.vertexSource, options.fragmentSource);
    
    let program = this.cache.get(key);
    if (!program) {
      program = new ShaderProgram(this.gl, options);
      this.cache.set(key, program);
    }
    
    return program;
  }

  set(vertexSource: string, fragmentSource: string, program: ShaderProgram): void {
    const key = this.generateKey(vertexSource, fragmentSource);
    
    // Dispose existing program if any
    const existing = this.cache.get(key);
    if (existing && existing !== program) {
      existing.dispose();
    }
    
    this.cache.set(key, program);
  }

  has(vertexSource: string, fragmentSource: string): boolean {
    const key = this.generateKey(vertexSource, fragmentSource);
    return this.cache.has(key);
  }

  remove(vertexSource: string, fragmentSource: string): void {
    const key = this.generateKey(vertexSource, fragmentSource);
    const program = this.cache.get(key);
    if (program) {
      program.dispose();
      this.cache.delete(key);
    }
  }

  getSize(): number {
    return this.cache.size;
  }

  clear(): void {
    this.cache.forEach(program => program.dispose());
    this.cache.clear();
  }

  dispose(): void {
    this.clear();
  }
}
