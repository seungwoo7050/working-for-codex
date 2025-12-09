/**
 * Texture Filter - 텍스처 필터링 및 샘플링
 * @version 3.1.2
 */

export type FilterMode = 'linear' | 'nearest' | 'mipmap';
export type WrapMode = 'repeat' | 'clamp' | 'mirror';

export interface TextureFilterOptions {
  minFilter: FilterMode;
  magFilter: Exclude<FilterMode, 'mipmap'>;
  wrapS: WrapMode;
  wrapT: WrapMode;
  anisotropy?: number;
}

export class TextureFilter {
  private gl: WebGL2RenderingContext;
  private maxAnisotropy: number;
  private anisotropyExt: EXT_texture_filter_anisotropic | null;

  constructor(gl: WebGL2RenderingContext) {
    this.gl = gl;
    this.anisotropyExt = gl.getExtension('EXT_texture_filter_anisotropic');
    this.maxAnisotropy = this.anisotropyExt 
      ? gl.getParameter(this.anisotropyExt.MAX_TEXTURE_MAX_ANISOTROPY_EXT)
      : 1;
  }

  apply(texture: WebGLTexture, options: Partial<TextureFilterOptions> = {}): void {
    const {
      minFilter = 'linear',
      magFilter = 'linear',
      wrapS = 'clamp',
      wrapT = 'clamp',
      anisotropy = 1,
    } = options;

    this.gl.bindTexture(this.gl.TEXTURE_2D, texture);

    // Min filter
    this.gl.texParameteri(
      this.gl.TEXTURE_2D,
      this.gl.TEXTURE_MIN_FILTER,
      this.getGLMinFilter(minFilter)
    );

    // Mag filter
    this.gl.texParameteri(
      this.gl.TEXTURE_2D,
      this.gl.TEXTURE_MAG_FILTER,
      this.getGLMagFilter(magFilter)
    );

    // Wrap modes
    this.gl.texParameteri(
      this.gl.TEXTURE_2D,
      this.gl.TEXTURE_WRAP_S,
      this.getGLWrap(wrapS)
    );

    this.gl.texParameteri(
      this.gl.TEXTURE_2D,
      this.gl.TEXTURE_WRAP_T,
      this.getGLWrap(wrapT)
    );

    // Anisotropic filtering
    if (this.anisotropyExt && anisotropy > 1) {
      const level = Math.min(anisotropy, this.maxAnisotropy);
      this.gl.texParameterf(
        this.gl.TEXTURE_2D,
        this.anisotropyExt.TEXTURE_MAX_ANISOTROPY_EXT,
        level
      );
    }
  }

  applyLinear(texture: WebGLTexture): void {
    this.apply(texture, { minFilter: 'linear', magFilter: 'linear' });
  }

  applyNearest(texture: WebGLTexture): void {
    this.apply(texture, { minFilter: 'nearest', magFilter: 'nearest' });
  }

  applyMipmap(texture: WebGLTexture): void {
    this.apply(texture, { minFilter: 'mipmap', magFilter: 'linear' });
  }

  private getGLMinFilter(mode: FilterMode): number {
    switch (mode) {
      case 'linear': return this.gl.LINEAR;
      case 'nearest': return this.gl.NEAREST;
      case 'mipmap': return this.gl.LINEAR_MIPMAP_LINEAR;
    }
  }

  private getGLMagFilter(mode: Exclude<FilterMode, 'mipmap'>): number {
    switch (mode) {
      case 'linear': return this.gl.LINEAR;
      case 'nearest': return this.gl.NEAREST;
    }
  }

  private getGLWrap(mode: WrapMode): number {
    switch (mode) {
      case 'repeat': return this.gl.REPEAT;
      case 'clamp': return this.gl.CLAMP_TO_EDGE;
      case 'mirror': return this.gl.MIRRORED_REPEAT;
    }
  }

  getMaxAnisotropy(): number {
    return this.maxAnisotropy;
  }

  isAnisotropySupported(): boolean {
    return this.anisotropyExt !== null;
  }
}
