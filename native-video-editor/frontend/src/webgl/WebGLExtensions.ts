/**
 * WebGL Extensions - 확장 감지 및 폴백
 * @version 3.1.0
 */

export interface ExtensionSupport {
  floatTextures: boolean;
  anisotropicFiltering: boolean;
  colorBufferFloat: boolean;
  debugRendererInfo: boolean;
  loseContext: boolean;
}

export class WebGLExtensions {
  private gl: WebGL2RenderingContext;
  private extensions: Map<string, unknown> = new Map();

  // 필수 확장 목록
  private readonly REQUIRED_EXTENSIONS: string[] = [];

  // 선택 확장 목록
  private readonly OPTIONAL_EXTENSIONS = [
    'EXT_color_buffer_float',
    'EXT_texture_filter_anisotropic',
    'WEBGL_debug_renderer_info',
    'WEBGL_lose_context',
    'OES_texture_float_linear',
  ];

  constructor(gl: WebGL2RenderingContext) {
    this.gl = gl;
    this.loadExtensions();
  }

  private loadExtensions(): void {
    // 필수 확장 로드 및 검증
    for (const name of this.REQUIRED_EXTENSIONS) {
      const ext = this.gl.getExtension(name);
      if (!ext) {
        throw new Error(`Required WebGL extension not supported: ${name}`);
      }
      this.extensions.set(name, ext);
    }

    // 선택 확장 로드 (실패해도 계속 진행)
    for (const name of this.OPTIONAL_EXTENSIONS) {
      const ext = this.gl.getExtension(name);
      if (ext) {
        this.extensions.set(name, ext);
      }
    }
  }

  getExtension<T>(name: string): T | null {
    if (this.extensions.has(name)) {
      return this.extensions.get(name) as T;
    }
    
    const ext = this.gl.getExtension(name);
    if (ext) {
      this.extensions.set(name, ext);
    }
    return ext as T | null;
  }

  hasExtension(name: string): boolean {
    return this.extensions.has(name) || this.gl.getExtension(name) !== null;
  }

  getSupport(): ExtensionSupport {
    return {
      floatTextures: this.hasExtension('OES_texture_float_linear'),
      anisotropicFiltering: this.hasExtension('EXT_texture_filter_anisotropic'),
      colorBufferFloat: this.hasExtension('EXT_color_buffer_float'),
      debugRendererInfo: this.hasExtension('WEBGL_debug_renderer_info'),
      loseContext: this.hasExtension('WEBGL_lose_context'),
    };
  }

  getRendererInfo(): { vendor: string; renderer: string } | null {
    const ext = this.getExtension<WEBGL_debug_renderer_info>('WEBGL_debug_renderer_info');
    if (!ext) {
      return null;
    }

    return {
      vendor: this.gl.getParameter(ext.UNMASKED_VENDOR_WEBGL),
      renderer: this.gl.getParameter(ext.UNMASKED_RENDERER_WEBGL),
    };
  }

  getMaxAnisotropy(): number {
    const ext = this.getExtension<EXT_texture_filter_anisotropic>('EXT_texture_filter_anisotropic');
    if (!ext) {
      return 1;
    }
    return this.gl.getParameter(ext.MAX_TEXTURE_MAX_ANISOTROPY_EXT);
  }

  // 컨텍스트 손실 시뮬레이션 (테스트용)
  simulateContextLoss(): void {
    const ext = this.getExtension<WEBGL_lose_context>('WEBGL_lose_context');
    if (ext) {
      ext.loseContext();
    }
  }

  restoreContext(): void {
    const ext = this.getExtension<WEBGL_lose_context>('WEBGL_lose_context');
    if (ext) {
      ext.restoreContext();
    }
  }
}
