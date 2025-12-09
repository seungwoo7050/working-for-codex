/**
 * WebGL Memory Manager - GPU 리소스 관리
 * @version 3.1.0
 */

export interface ResourceStats {
  textures: number;
  buffers: number;
  programs: number;
  framebuffers: number;
  renderbuffers: number;
  vertexArrays: number;
  estimatedBytes: number;
}

interface TextureInfo {
  texture: WebGLTexture;
  width: number;
  height: number;
  format: number;
}

export class WebGLMemoryManager {
  private gl: WebGL2RenderingContext;
  private textures: Map<WebGLTexture, TextureInfo> = new Map();
  private buffers: Map<WebGLBuffer, number> = new Map(); // buffer -> size in bytes
  private programs: Set<WebGLProgram> = new Set();
  private framebuffers: Set<WebGLFramebuffer> = new Set();
  private renderbuffers: Set<WebGLRenderbuffer> = new Set();
  private vertexArrays: Set<WebGLVertexArrayObject> = new Set();

  constructor(gl: WebGL2RenderingContext) {
    this.gl = gl;
  }

  // Texture management
  createTexture(width: number, height: number, format: number = this.gl.RGBA): WebGLTexture {
    const texture = this.gl.createTexture();
    if (!texture) {
      throw new Error('Failed to create texture');
    }
    
    this.textures.set(texture, { texture, width, height, format });
    return texture;
  }

  trackTexture(texture: WebGLTexture, width: number, height: number, format: number = this.gl.RGBA): void {
    this.textures.set(texture, { texture, width, height, format });
  }

  deleteTexture(texture: WebGLTexture): void {
    if (this.textures.has(texture)) {
      this.gl.deleteTexture(texture);
      this.textures.delete(texture);
    }
  }

  // Buffer management
  createBuffer(sizeInBytes?: number): WebGLBuffer {
    const buffer = this.gl.createBuffer();
    if (!buffer) {
      throw new Error('Failed to create buffer');
    }
    
    this.buffers.set(buffer, sizeInBytes ?? 0);
    return buffer;
  }

  trackBuffer(buffer: WebGLBuffer, sizeInBytes: number): void {
    this.buffers.set(buffer, sizeInBytes);
  }

  deleteBuffer(buffer: WebGLBuffer): void {
    if (this.buffers.has(buffer)) {
      this.gl.deleteBuffer(buffer);
      this.buffers.delete(buffer);
    }
  }

  // Program management
  trackProgram(program: WebGLProgram): void {
    this.programs.add(program);
  }

  deleteProgram(program: WebGLProgram): void {
    if (this.programs.has(program)) {
      this.gl.deleteProgram(program);
      this.programs.delete(program);
    }
  }

  // Framebuffer management
  createFramebuffer(): WebGLFramebuffer {
    const framebuffer = this.gl.createFramebuffer();
    if (!framebuffer) {
      throw new Error('Failed to create framebuffer');
    }
    
    this.framebuffers.add(framebuffer);
    return framebuffer;
  }

  deleteFramebuffer(framebuffer: WebGLFramebuffer): void {
    if (this.framebuffers.has(framebuffer)) {
      this.gl.deleteFramebuffer(framebuffer);
      this.framebuffers.delete(framebuffer);
    }
  }

  // Renderbuffer management
  createRenderbuffer(): WebGLRenderbuffer {
    const renderbuffer = this.gl.createRenderbuffer();
    if (!renderbuffer) {
      throw new Error('Failed to create renderbuffer');
    }
    
    this.renderbuffers.add(renderbuffer);
    return renderbuffer;
  }

  deleteRenderbuffer(renderbuffer: WebGLRenderbuffer): void {
    if (this.renderbuffers.has(renderbuffer)) {
      this.gl.deleteRenderbuffer(renderbuffer);
      this.renderbuffers.delete(renderbuffer);
    }
  }

  // VAO management
  createVertexArray(): WebGLVertexArrayObject {
    const vao = this.gl.createVertexArray();
    if (!vao) {
      throw new Error('Failed to create vertex array');
    }
    
    this.vertexArrays.add(vao);
    return vao;
  }

  deleteVertexArray(vao: WebGLVertexArrayObject): void {
    if (this.vertexArrays.has(vao)) {
      this.gl.deleteVertexArray(vao);
      this.vertexArrays.delete(vao);
    }
  }

  // Statistics
  getStats(): ResourceStats {
    return {
      textures: this.textures.size,
      buffers: this.buffers.size,
      programs: this.programs.size,
      framebuffers: this.framebuffers.size,
      renderbuffers: this.renderbuffers.size,
      vertexArrays: this.vertexArrays.size,
      estimatedBytes: this.estimateMemoryUsage(),
    };
  }

  private estimateMemoryUsage(): number {
    let bytes = 0;

    // Texture memory
    this.textures.forEach((info) => {
      const bytesPerPixel = this.getBytesPerPixel(info.format);
      bytes += info.width * info.height * bytesPerPixel;
    });

    // Buffer memory
    this.buffers.forEach((size) => {
      bytes += size;
    });

    return bytes;
  }

  private getBytesPerPixel(format: number): number {
    switch (format) {
      case this.gl.RGBA:
      case this.gl.RGBA8:
        return 4;
      case this.gl.RGB:
      case this.gl.RGB8:
        return 3;
      case this.gl.RG:
      case this.gl.RG8:
        return 2;
      case this.gl.RED:
      case this.gl.R8:
        return 1;
      case this.gl.RGBA16F:
        return 8;
      case this.gl.RGBA32F:
        return 16;
      default:
        return 4;
    }
  }

  // Cleanup
  disposeAll(): void {
    // Delete all tracked resources
    this.textures.forEach((info) => this.gl.deleteTexture(info.texture));
    this.textures.clear();

    this.buffers.forEach((_, buffer) => this.gl.deleteBuffer(buffer));
    this.buffers.clear();

    this.programs.forEach((program) => this.gl.deleteProgram(program));
    this.programs.clear();

    this.framebuffers.forEach((fb) => this.gl.deleteFramebuffer(fb));
    this.framebuffers.clear();

    this.renderbuffers.forEach((rb) => this.gl.deleteRenderbuffer(rb));
    this.renderbuffers.clear();

    this.vertexArrays.forEach((vao) => this.gl.deleteVertexArray(vao));
    this.vertexArrays.clear();
  }
}
