/**
 * Texture Compressor - 압축 텍스처 지원
 * @version 3.1.2
 */

export interface CompressionInfo {
  supported: boolean;
  formats: string[];
  extensions: string[];
}

export class TextureCompressor {
  private gl: WebGL2RenderingContext;
  private s3tcExt: WEBGL_compressed_texture_s3tc | null;
  private etc1Ext: WEBGL_compressed_texture_etc1 | null;
  private astcExt: WEBGL_compressed_texture_astc | null;

  constructor(gl: WebGL2RenderingContext) {
    this.gl = gl;
    this.s3tcExt = gl.getExtension('WEBGL_compressed_texture_s3tc');
    this.etc1Ext = gl.getExtension('WEBGL_compressed_texture_etc1');
    this.astcExt = gl.getExtension('WEBGL_compressed_texture_astc');
  }

  getCompressionInfo(): CompressionInfo {
    const formats: string[] = [];
    const extensions: string[] = [];

    if (this.s3tcExt) {
      extensions.push('WEBGL_compressed_texture_s3tc');
      formats.push('DXT1', 'DXT3', 'DXT5');
    }

    if (this.etc1Ext) {
      extensions.push('WEBGL_compressed_texture_etc1');
      formats.push('ETC1');
    }

    if (this.astcExt) {
      extensions.push('WEBGL_compressed_texture_astc');
      formats.push('ASTC');
    }

    return {
      supported: formats.length > 0,
      formats,
      extensions,
    };
  }

  isS3TCSupported(): boolean {
    return this.s3tcExt !== null;
  }

  isETC1Supported(): boolean {
    return this.etc1Ext !== null;
  }

  isASTCSupported(): boolean {
    return this.astcExt !== null;
  }

  getS3TCFormat(type: 'DXT1' | 'DXT1_ALPHA' | 'DXT3' | 'DXT5'): number | null {
    if (!this.s3tcExt) return null;

    switch (type) {
      case 'DXT1': return this.s3tcExt.COMPRESSED_RGB_S3TC_DXT1_EXT;
      case 'DXT1_ALPHA': return this.s3tcExt.COMPRESSED_RGBA_S3TC_DXT1_EXT;
      case 'DXT3': return this.s3tcExt.COMPRESSED_RGBA_S3TC_DXT3_EXT;
      case 'DXT5': return this.s3tcExt.COMPRESSED_RGBA_S3TC_DXT5_EXT;
    }
  }

  /**
   * Load compressed texture data
   */
  loadCompressed(
    texture: WebGLTexture,
    width: number,
    height: number,
    format: number,
    data: ArrayBufferView
  ): void {
    this.gl.bindTexture(this.gl.TEXTURE_2D, texture);
    this.gl.compressedTexImage2D(
      this.gl.TEXTURE_2D,
      0,
      format,
      width,
      height,
      0,
      data
    );
  }

  /**
   * Estimate memory savings from compression
   */
  estimateCompressionRatio(format: string): number {
    switch (format) {
      case 'DXT1': return 6; // 6:1 compression
      case 'DXT5': return 4; // 4:1 compression
      case 'ETC1': return 6;
      case 'ASTC': return 4; // varies by block size
      default: return 1;
    }
  }

  /**
   * Get recommended format for current platform
   */
  getRecommendedFormat(): string | null {
    // S3TC is widely supported on desktop
    if (this.s3tcExt) return 'DXT5';
    // ETC1 for older mobile
    if (this.etc1Ext) return 'ETC1';
    // ASTC for modern mobile
    if (this.astcExt) return 'ASTC';
    return null;
  }
}
