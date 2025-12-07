/**
 * Framebuffer - 오프스크린 렌더링
 * @version 3.1.3
 */

export interface FramebufferOptions {
  width: number;
  height: number;
  colorFormat?: number;
  depthBuffer?: boolean;
  stencilBuffer?: boolean;
}

export class Framebuffer {
  private gl: WebGL2RenderingContext;
  private fbo: WebGLFramebuffer;
  private colorTexture: WebGLTexture;
  private depthRenderbuffer: WebGLRenderbuffer | null = null;
  private width: number;
  private height: number;
  private isDisposed = false;

  constructor(gl: WebGL2RenderingContext, options: FramebufferOptions) {
    this.gl = gl;
    this.width = options.width;
    this.height = options.height;

    // Create framebuffer
    const fbo = gl.createFramebuffer();
    if (!fbo) {
      throw new Error('Failed to create framebuffer');
    }
    this.fbo = fbo;

    // Create color attachment
    const colorTexture = gl.createTexture();
    if (!colorTexture) {
      gl.deleteFramebuffer(fbo);
      throw new Error('Failed to create color texture');
    }
    this.colorTexture = colorTexture;

    // Setup color texture
    gl.bindTexture(gl.TEXTURE_2D, colorTexture);
    gl.texImage2D(
      gl.TEXTURE_2D,
      0,
      options.colorFormat ?? gl.RGBA8,
      options.width,
      options.height,
      0,
      gl.RGBA,
      gl.UNSIGNED_BYTE,
      null
    );
    gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.LINEAR);
    gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MAG_FILTER, gl.LINEAR);
    gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_S, gl.CLAMP_TO_EDGE);
    gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_T, gl.CLAMP_TO_EDGE);

    // Attach to framebuffer
    gl.bindFramebuffer(gl.FRAMEBUFFER, fbo);
    gl.framebufferTexture2D(
      gl.FRAMEBUFFER,
      gl.COLOR_ATTACHMENT0,
      gl.TEXTURE_2D,
      colorTexture,
      0
    );

    // Create depth/stencil buffer if requested
    if (options.depthBuffer || options.stencilBuffer) {
      const rbo = gl.createRenderbuffer();
      if (!rbo) {
        throw new Error('Failed to create renderbuffer');
      }
      this.depthRenderbuffer = rbo;

      gl.bindRenderbuffer(gl.RENDERBUFFER, rbo);
      
      const format = options.stencilBuffer 
        ? gl.DEPTH24_STENCIL8 
        : gl.DEPTH_COMPONENT24;
      
      gl.renderbufferStorage(gl.RENDERBUFFER, format, options.width, options.height);

      const attachment = options.stencilBuffer 
        ? gl.DEPTH_STENCIL_ATTACHMENT 
        : gl.DEPTH_ATTACHMENT;
      
      gl.framebufferRenderbuffer(gl.FRAMEBUFFER, attachment, gl.RENDERBUFFER, rbo);
    }

    // Check completeness
    const status = gl.checkFramebufferStatus(gl.FRAMEBUFFER);
    if (status !== gl.FRAMEBUFFER_COMPLETE) {
      this.dispose();
      throw new Error(`Framebuffer not complete: ${status}`);
    }

    // Unbind
    gl.bindFramebuffer(gl.FRAMEBUFFER, null);
  }

  bind(): void {
    this.gl.bindFramebuffer(this.gl.FRAMEBUFFER, this.fbo);
    this.gl.viewport(0, 0, this.width, this.height);
  }

  unbind(): void {
    this.gl.bindFramebuffer(this.gl.FRAMEBUFFER, null);
  }

  getColorTexture(): WebGLTexture {
    return this.colorTexture;
  }

  getFramebuffer(): WebGLFramebuffer {
    return this.fbo;
  }

  getWidth(): number {
    return this.width;
  }

  getHeight(): number {
    return this.height;
  }

  resize(width: number, height: number): void {
    this.width = width;
    this.height = height;

    // Resize color texture
    this.gl.bindTexture(this.gl.TEXTURE_2D, this.colorTexture);
    this.gl.texImage2D(
      this.gl.TEXTURE_2D,
      0,
      this.gl.RGBA8,
      width,
      height,
      0,
      this.gl.RGBA,
      this.gl.UNSIGNED_BYTE,
      null
    );

    // Resize depth buffer if exists
    if (this.depthRenderbuffer) {
      this.gl.bindRenderbuffer(this.gl.RENDERBUFFER, this.depthRenderbuffer);
      this.gl.renderbufferStorage(
        this.gl.RENDERBUFFER,
        this.gl.DEPTH_COMPONENT24,
        width,
        height
      );
    }
  }

  dispose(): void {
    if (this.isDisposed) return;

    this.gl.deleteTexture(this.colorTexture);
    if (this.depthRenderbuffer) {
      this.gl.deleteRenderbuffer(this.depthRenderbuffer);
    }
    this.gl.deleteFramebuffer(this.fbo);
    
    this.isDisposed = true;
  }
}
