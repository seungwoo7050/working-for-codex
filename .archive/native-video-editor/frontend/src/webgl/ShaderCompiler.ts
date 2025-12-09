/**
 * Shader Compiler - GLSL 셰이더 컴파일
 * @version 3.1.1
 */

export type ShaderType = 'vertex' | 'fragment';

export interface CompileResult {
  shader: WebGLShader | null;
  success: boolean;
  errors: string[];
}

export class ShaderCompiler {
  private gl: WebGL2RenderingContext;

  constructor(gl: WebGL2RenderingContext) {
    this.gl = gl;
  }

  compile(source: string, type: ShaderType): CompileResult {
    const shaderType = type === 'vertex' 
      ? this.gl.VERTEX_SHADER 
      : this.gl.FRAGMENT_SHADER;

    const shader = this.gl.createShader(shaderType);
    if (!shader) {
      return { 
        shader: null, 
        success: false, 
        errors: ['Failed to create shader object'] 
      };
    }

    this.gl.shaderSource(shader, source);
    this.gl.compileShader(shader);

    if (!this.gl.getShaderParameter(shader, this.gl.COMPILE_STATUS)) {
      const log = this.gl.getShaderInfoLog(shader) || 'Unknown compilation error';
      const errors = this.parseErrors(log, type);
      this.gl.deleteShader(shader);
      return { shader: null, success: false, errors };
    }

    return { shader, success: true, errors: [] };
  }

  compileVertex(source: string): CompileResult {
    return this.compile(source, 'vertex');
  }

  compileFragment(source: string): CompileResult {
    return this.compile(source, 'fragment');
  }

  private parseErrors(log: string, type: ShaderType): string[] {
    const lines = log.split('\n').filter(line => line.trim().length > 0);
    return lines.map(line => `[${type}] ${line}`);
  }

  // 기본 버텍스 셰이더 (풀스크린 쿼드)
  static getDefaultVertexShader(): string {
    return `#version 300 es
      in vec2 aPosition;
      in vec2 aTexCoord;
      out vec2 vTexCoord;
      
      void main() {
        vTexCoord = aTexCoord;
        gl_Position = vec4(aPosition, 0.0, 1.0);
      }
    `;
  }

  // 기본 프래그먼트 셰이더 (텍스처 샘플링)
  static getDefaultFragmentShader(): string {
    return `#version 300 es
      precision highp float;
      
      uniform sampler2D uTexture;
      in vec2 vTexCoord;
      out vec4 fragColor;
      
      void main() {
        fragColor = texture(uTexture, vTexCoord);
      }
    `;
  }
}
