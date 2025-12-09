/**
 * Renderbuffer - 깊이/스텐실 버퍼 관리
 * @version 3.1.3
 */

export type RenderbufferFormat = 'depth16' | 'depth24' | 'depth32f' | 'depth24stencil8' | 'stencil8';

export class Renderbuffer {
  private gl: WebGL2RenderingContext;
  private rbo: WebGLRenderbuffer;
  private width: number;
  private height: number;
  private format: RenderbufferFormat;
  private isDisposed = false;

  constructor(
    gl: WebGL2RenderingContext,
    width: number,
    height: number,
    format: RenderbufferFormat = 'depth24'
  ) {
    this.gl = gl;
    this.width = width;
    this.height = height;
    this.format = format;

    const rbo = gl.createRenderbuffer();
    if (!rbo) {
      throw new Error('Failed to create renderbuffer');
    }
    this.rbo = rbo;

    this.allocate();
  }

  private allocate(): void {
    this.gl.bindRenderbuffer(this.gl.RENDERBUFFER, this.rbo);
    this.gl.renderbufferStorage(
      this.gl.RENDERBUFFER,
      this.getInternalFormat(),
      this.width,
      this.height
    );
  }

  private getInternalFormat(): number {
    switch (this.format) {
      case 'depth16': return this.gl.DEPTH_COMPONENT16;
      case 'depth24': return this.gl.DEPTH_COMPONENT24;
      case 'depth32f': return this.gl.DEPTH_COMPONENT32F;
      case 'depth24stencil8': return this.gl.DEPTH24_STENCIL8;
      case 'stencil8': return this.gl.STENCIL_INDEX8;
    }
  }

  getAttachmentType(): number {
    switch (this.format) {
      case 'depth16':
      case 'depth24':
      case 'depth32f':
        return this.gl.DEPTH_ATTACHMENT;
      case 'depth24stencil8':
        return this.gl.DEPTH_STENCIL_ATTACHMENT;
      case 'stencil8':
        return this.gl.STENCIL_ATTACHMENT;
    }
  }

  bind(): void {
    this.gl.bindRenderbuffer(this.gl.RENDERBUFFER, this.rbo);
  }

  unbind(): void {
    this.gl.bindRenderbuffer(this.gl.RENDERBUFFER, null);
  }

  getRenderbuffer(): WebGLRenderbuffer {
    return this.rbo;
  }

  resize(width: number, height: number): void {
    this.width = width;
    this.height = height;
    this.allocate();
  }

  getWidth(): number {
    return this.width;
  }

  getHeight(): number {
    return this.height;
  }

  getFormat(): RenderbufferFormat {
    return this.format;
  }

  dispose(): void {
    if (this.isDisposed) return;
    
    this.gl.deleteRenderbuffer(this.rbo);
    this.isDisposed = true;
  }
}
