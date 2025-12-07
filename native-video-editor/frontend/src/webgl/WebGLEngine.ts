/**
 * WebGL Engine - WebGL2 컨텍스트 관리
 * @version 3.1.0
 */

export interface WebGLEngineOptions {
  antialias?: boolean;
  alpha?: boolean;
  depth?: boolean;
  stencil?: boolean;
  preserveDrawingBuffer?: boolean;
  powerPreference?: 'default' | 'high-performance' | 'low-power';
}

export class WebGLEngine {
  private gl: WebGL2RenderingContext;
  private canvas: HTMLCanvasElement;
  private isDisposed = false;

  constructor(canvas: HTMLCanvasElement, options?: WebGLEngineOptions) {
    this.canvas = canvas;

    const contextAttributes: WebGLContextAttributes = {
      antialias: options?.antialias ?? true,
      alpha: options?.alpha ?? false,
      depth: options?.depth ?? true,
      stencil: options?.stencil ?? false,
      preserveDrawingBuffer: options?.preserveDrawingBuffer ?? false,
      powerPreference: options?.powerPreference ?? 'high-performance',
    };

    const gl = canvas.getContext('webgl2', contextAttributes);
    if (!gl) {
      throw new Error('WebGL2 is not supported in this browser');
    }

    this.gl = gl;
    this.setupContextLossHandling();
  }

  private setupContextLossHandling(): void {
    this.canvas.addEventListener('webglcontextlost', (event) => {
      event.preventDefault();
      console.warn('[WebGLEngine] Context lost');
    });

    this.canvas.addEventListener('webglcontextrestored', () => {
      console.log('[WebGLEngine] Context restored');
      // Re-initialize resources here if needed
    });
  }

  getContext(): WebGL2RenderingContext {
    if (this.isDisposed) {
      throw new Error('WebGLEngine has been disposed');
    }
    return this.gl;
  }

  getCanvas(): HTMLCanvasElement {
    return this.canvas;
  }

  resize(width: number, height: number): void {
    this.canvas.width = width;
    this.canvas.height = height;
    this.gl.viewport(0, 0, width, height);
  }

  clear(r = 0, g = 0, b = 0, a = 1): void {
    this.gl.clearColor(r, g, b, a);
    this.gl.clear(this.gl.COLOR_BUFFER_BIT | this.gl.DEPTH_BUFFER_BIT);
  }

  isContextLost(): boolean {
    return this.gl.isContextLost();
  }

  dispose(): void {
    if (this.isDisposed) return;
    
    // WebGL context will be garbage collected with canvas
    // but we mark as disposed to prevent further use
    this.isDisposed = true;
  }
}
