/**
 * Shader Program - 셰이더 프로그램 관리
 * @version 3.1.1
 */

import { ShaderCompiler } from './ShaderCompiler';

export interface ShaderProgramOptions {
  vertexSource: string;
  fragmentSource: string;
}

export class ShaderProgram {
  private gl: WebGL2RenderingContext;
  private program: WebGLProgram;
  private uniformLocations: Map<string, WebGLUniformLocation> = new Map();
  private attributeLocations: Map<string, number> = new Map();
  private isDisposed = false;

  constructor(gl: WebGL2RenderingContext, options: ShaderProgramOptions) {
    this.gl = gl;
    this.program = this.createProgram(options.vertexSource, options.fragmentSource);
  }

  private createProgram(vertexSource: string, fragmentSource: string): WebGLProgram {
    const compiler = new ShaderCompiler(this.gl);

    // Compile vertex shader
    const vertexResult = compiler.compileVertex(vertexSource);
    if (!vertexResult.success || !vertexResult.shader) {
      throw new Error(`Vertex shader compilation failed:\n${vertexResult.errors.join('\n')}`);
    }

    // Compile fragment shader
    const fragmentResult = compiler.compileFragment(fragmentSource);
    if (!fragmentResult.success || !fragmentResult.shader) {
      this.gl.deleteShader(vertexResult.shader);
      throw new Error(`Fragment shader compilation failed:\n${fragmentResult.errors.join('\n')}`);
    }

    // Create and link program
    const program = this.gl.createProgram();
    if (!program) {
      this.gl.deleteShader(vertexResult.shader);
      this.gl.deleteShader(fragmentResult.shader);
      throw new Error('Failed to create shader program');
    }

    this.gl.attachShader(program, vertexResult.shader);
    this.gl.attachShader(program, fragmentResult.shader);
    this.gl.linkProgram(program);

    // Check link status
    if (!this.gl.getProgramParameter(program, this.gl.LINK_STATUS)) {
      const log = this.gl.getProgramInfoLog(program) || 'Unknown link error';
      this.gl.deleteShader(vertexResult.shader);
      this.gl.deleteShader(fragmentResult.shader);
      this.gl.deleteProgram(program);
      throw new Error(`Shader program linking failed: ${log}`);
    }

    // Clean up shaders (they're now part of the program)
    this.gl.deleteShader(vertexResult.shader);
    this.gl.deleteShader(fragmentResult.shader);

    return program;
  }

  use(): void {
    if (this.isDisposed) {
      throw new Error('ShaderProgram has been disposed');
    }
    this.gl.useProgram(this.program);
  }

  getProgram(): WebGLProgram {
    return this.program;
  }

  getUniformLocation(name: string): WebGLUniformLocation | null {
    if (this.uniformLocations.has(name)) {
      return this.uniformLocations.get(name)!;
    }

    const location = this.gl.getUniformLocation(this.program, name);
    if (location) {
      this.uniformLocations.set(name, location);
    }
    return location;
  }

  getAttributeLocation(name: string): number {
    if (this.attributeLocations.has(name)) {
      return this.attributeLocations.get(name)!;
    }

    const location = this.gl.getAttribLocation(this.program, name);
    this.attributeLocations.set(name, location);
    return location;
  }

  // Uniform setters
  setUniform1i(name: string, value: number): void {
    const location = this.getUniformLocation(name);
    if (location) {
      this.gl.uniform1i(location, value);
    }
  }

  setUniform1f(name: string, value: number): void {
    const location = this.getUniformLocation(name);
    if (location) {
      this.gl.uniform1f(location, value);
    }
  }

  setUniform2f(name: string, x: number, y: number): void {
    const location = this.getUniformLocation(name);
    if (location) {
      this.gl.uniform2f(location, x, y);
    }
  }

  setUniform3f(name: string, x: number, y: number, z: number): void {
    const location = this.getUniformLocation(name);
    if (location) {
      this.gl.uniform3f(location, x, y, z);
    }
  }

  setUniform4f(name: string, x: number, y: number, z: number, w: number): void {
    const location = this.getUniformLocation(name);
    if (location) {
      this.gl.uniform4f(location, x, y, z, w);
    }
  }

  setUniformMatrix4fv(name: string, matrix: Float32Array): void {
    const location = this.getUniformLocation(name);
    if (location) {
      this.gl.uniformMatrix4fv(location, false, matrix);
    }
  }

  dispose(): void {
    if (this.isDisposed) return;
    
    this.gl.deleteProgram(this.program);
    this.uniformLocations.clear();
    this.attributeLocations.clear();
    this.isDisposed = true;
  }
}
