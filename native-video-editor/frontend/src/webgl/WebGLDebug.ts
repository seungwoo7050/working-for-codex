/**
 * WebGL Debug - 디버깅 유틸리티
 * @version 3.1.0
 */

export interface WebGLDebugOptions {
  enabled?: boolean;
  throwOnError?: boolean;
  logCalls?: boolean;
}

const GL_ERROR_NAMES: Record<number, string> = {
  0x0500: 'INVALID_ENUM',
  0x0501: 'INVALID_VALUE',
  0x0502: 'INVALID_OPERATION',
  0x0505: 'OUT_OF_MEMORY',
  0x0506: 'INVALID_FRAMEBUFFER_OPERATION',
  0x9242: 'CONTEXT_LOST_WEBGL',
};

export class WebGLDebug {
  private gl: WebGL2RenderingContext;
  private enabled: boolean;
  private throwOnError: boolean;
  constructor(gl: WebGL2RenderingContext, options?: WebGLDebugOptions) {
    this.gl = gl;
    this.enabled = options?.enabled ?? true;
    this.throwOnError = options?.throwOnError ?? false;
    // logCalls option reserved for future verbose logging feature
  }

  checkError(operation: string): boolean {
    if (!this.enabled) return true;

    const error = this.gl.getError();
    if (error === this.gl.NO_ERROR) {
      return true;
    }

    const errorName = GL_ERROR_NAMES[error] || `UNKNOWN_ERROR(${error})`;
    const message = `[WebGL Error] ${operation}: ${errorName}`;

    console.error(message);

    if (this.throwOnError) {
      throw new Error(message);
    }

    return false;
  }

  checkFramebufferStatus(target: number = this.gl.FRAMEBUFFER): boolean {
    if (!this.enabled) return true;

    const status = this.gl.checkFramebufferStatus(target);
    if (status === this.gl.FRAMEBUFFER_COMPLETE) {
      return true;
    }

    const statusNames: Record<number, string> = {
      [this.gl.FRAMEBUFFER_INCOMPLETE_ATTACHMENT]: 'INCOMPLETE_ATTACHMENT',
      [this.gl.FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT]: 'INCOMPLETE_MISSING_ATTACHMENT',
      [this.gl.FRAMEBUFFER_INCOMPLETE_DIMENSIONS]: 'INCOMPLETE_DIMENSIONS',
      [this.gl.FRAMEBUFFER_UNSUPPORTED]: 'UNSUPPORTED',
      [this.gl.FRAMEBUFFER_INCOMPLETE_MULTISAMPLE]: 'INCOMPLETE_MULTISAMPLE',
    };

    const statusName = statusNames[status] || `UNKNOWN(${status})`;
    console.error(`[WebGL Error] Framebuffer: ${statusName}`);

    return false;
  }

  validateProgram(program: WebGLProgram): boolean {
    this.gl.validateProgram(program);
    const valid = this.gl.getProgramParameter(program, this.gl.VALIDATE_STATUS);
    
    if (!valid && this.enabled) {
      const log = this.gl.getProgramInfoLog(program);
      console.error(`[WebGL Error] Program validation failed: ${log}`);
    }

    return valid;
  }

  getShaderCompileError(shader: WebGLShader): string | null {
    const compiled = this.gl.getShaderParameter(shader, this.gl.COMPILE_STATUS);
    if (compiled) {
      return null;
    }
    return this.gl.getShaderInfoLog(shader);
  }

  getProgramLinkError(program: WebGLProgram): string | null {
    const linked = this.gl.getProgramParameter(program, this.gl.LINK_STATUS);
    if (linked) {
      return null;
    }
    return this.gl.getProgramInfoLog(program);
  }

  log(message: string, data?: unknown): void {
    if (!this.enabled) return;
    
    if (data !== undefined) {
      console.log(`[WebGL] ${message}`, data);
    } else {
      console.log(`[WebGL] ${message}`);
    }
  }

  warn(message: string, data?: unknown): void {
    if (!this.enabled) return;
    
    if (data !== undefined) {
      console.warn(`[WebGL] ${message}`, data);
    } else {
      console.warn(`[WebGL] ${message}`);
    }
  }

  getContextInfo(): object {
    return {
      vendor: this.gl.getParameter(this.gl.VENDOR),
      renderer: this.gl.getParameter(this.gl.RENDERER),
      version: this.gl.getParameter(this.gl.VERSION),
      shadingLanguageVersion: this.gl.getParameter(this.gl.SHADING_LANGUAGE_VERSION),
      maxTextureSize: this.gl.getParameter(this.gl.MAX_TEXTURE_SIZE),
      maxViewportDims: this.gl.getParameter(this.gl.MAX_VIEWPORT_DIMS),
      maxTextureImageUnits: this.gl.getParameter(this.gl.MAX_TEXTURE_IMAGE_UNITS),
      maxVertexTextureImageUnits: this.gl.getParameter(this.gl.MAX_VERTEX_TEXTURE_IMAGE_UNITS),
      maxCombinedTextureImageUnits: this.gl.getParameter(this.gl.MAX_COMBINED_TEXTURE_IMAGE_UNITS),
    };
  }

  setEnabled(enabled: boolean): void {
    this.enabled = enabled;
  }

  isEnabled(): boolean {
    return this.enabled;
  }
}
