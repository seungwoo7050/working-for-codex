/**
 * Mipmap Generator - LOD 지원을 위한 Mipmap 생성
 * @version 3.1.2
 */

export class MipmapGenerator {
  private gl: WebGL2RenderingContext;

  constructor(gl: WebGL2RenderingContext) {
    this.gl = gl;
  }

  generate(texture: WebGLTexture): void {
    this.gl.bindTexture(this.gl.TEXTURE_2D, texture);
    this.gl.generateMipmap(this.gl.TEXTURE_2D);
  }

  generateForCubemap(texture: WebGLTexture): void {
    this.gl.bindTexture(this.gl.TEXTURE_CUBE_MAP, texture);
    this.gl.generateMipmap(this.gl.TEXTURE_CUBE_MAP);
  }

  /**
   * Check if dimensions are power of two (required for mipmaps in some cases)
   */
  isPowerOfTwo(width: number, height: number): boolean {
    return this.isPOT(width) && this.isPOT(height);
  }

  private isPOT(value: number): boolean {
    return value > 0 && (value & (value - 1)) === 0;
  }

  /**
   * Get the next power of two >= value
   */
  nextPowerOfTwo(value: number): number {
    if (this.isPOT(value)) return value;
    
    let pot = 1;
    while (pot < value) {
      pot *= 2;
    }
    return pot;
  }

  /**
   * Calculate number of mipmap levels for given dimensions
   */
  calculateMipLevels(width: number, height: number): number {
    return Math.floor(Math.log2(Math.max(width, height))) + 1;
  }

  /**
   * Get size of texture at specific mip level
   */
  getMipSize(baseWidth: number, baseHeight: number, level: number): { width: number; height: number } {
    const divisor = Math.pow(2, level);
    return {
      width: Math.max(1, Math.floor(baseWidth / divisor)),
      height: Math.max(1, Math.floor(baseHeight / divisor)),
    };
  }
}
