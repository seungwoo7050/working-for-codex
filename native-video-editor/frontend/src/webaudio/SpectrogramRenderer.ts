/**
 * Spectrogram Renderer - 스펙트로그램 렌더링
 * @version 3.2.3
 */

export interface SpectrogramStyle {
  colorMap?: 'viridis' | 'magma' | 'plasma' | 'grayscale' | 'rainbow';
  backgroundColor?: string;
  minDecibels?: number;
  maxDecibels?: number;
}

export class SpectrogramRenderer {
  private canvas: HTMLCanvasElement;
  private ctx: CanvasRenderingContext2D;
  private style: Required<SpectrogramStyle>;
  private imageData: ImageData;

  constructor(canvas: HTMLCanvasElement, style: SpectrogramStyle = {}) {
    this.canvas = canvas;
    const ctx = canvas.getContext('2d');
    if (!ctx) {
      throw new Error('Failed to get 2D context');
    }
    this.ctx = ctx;

    this.style = {
      colorMap: style.colorMap ?? 'viridis',
      backgroundColor: style.backgroundColor ?? '#000000',
      minDecibels: style.minDecibels ?? -100,
      maxDecibels: style.maxDecibels ?? -30,
    };

    this.imageData = ctx.createImageData(canvas.width, canvas.height);
    this.clear();
  }

  setStyle(style: Partial<SpectrogramStyle>): void {
    Object.assign(this.style, style);
  }

  /**
   * 주파수 데이터 한 컬럼 추가 (스크롤링)
   */
  addColumn(frequencyData: Uint8Array): void {
    const { width, height } = this.canvas;
    const data = this.imageData.data;

    // Shift existing data left
    for (let y = 0; y < height; y++) {
      const rowOffset = y * width * 4;
      for (let x = 0; x < width - 1; x++) {
        const dstIdx = rowOffset + x * 4;
        const srcIdx = rowOffset + (x + 1) * 4;
        data[dstIdx] = data[srcIdx];
        data[dstIdx + 1] = data[srcIdx + 1];
        data[dstIdx + 2] = data[srcIdx + 2];
        data[dstIdx + 3] = data[srcIdx + 3];
      }
    }

    // Add new column at the right
    const x = width - 1;
    const binHeight = frequencyData.length;

    for (let y = 0; y < height; y++) {
      const binIndex = Math.floor((1 - y / height) * binHeight);
      const value = frequencyData[binIndex] / 255;
      const color = this.getColor(value);

      const idx = (y * width + x) * 4;
      data[idx] = color.r;
      data[idx + 1] = color.g;
      data[idx + 2] = color.b;
      data[idx + 3] = 255;
    }

    this.ctx.putImageData(this.imageData, 0, 0);
  }

  /**
   * 전체 버퍼 스펙트로그램 렌더링
   */
  renderBuffer(
    buffer: AudioBuffer,
    fftSize: number = 2048,
    hopSize?: number
  ): void {
    // Note: Full offline analysis requires OfflineAudioContext with AnalyserNode
    // This is a placeholder for the full implementation
    
    hopSize = hopSize ?? fftSize / 4;
    hopSize = hopSize ?? fftSize / 4;
    const { width } = this.canvas;

    // Calculate how many FFT frames we need (for future implementation)
    const _frameCount = Math.floor((buffer.length - fftSize) / hopSize);
    void _frameCount; // Suppress unused warning
    void width;

    // For now, use a simplified approach
    this.clear();
    console.warn('Full buffer spectrogram requires OfflineAudioContext analysis');
  }

  private getColor(value: number): { r: number; g: number; b: number } {
    switch (this.style.colorMap) {
      case 'grayscale':
        return this.grayscale(value);
      case 'rainbow':
        return this.rainbow(value);
      case 'magma':
        return this.magma(value);
      case 'plasma':
        return this.plasma(value);
      case 'viridis':
      default:
        return this.viridis(value);
    }
  }

  private grayscale(t: number): { r: number; g: number; b: number } {
    const v = Math.floor(t * 255);
    return { r: v, g: v, b: v };
  }

  private rainbow(t: number): { r: number; g: number; b: number } {
    const h = t * 360;
    const s = 1;
    const l = 0.5;
    
    const c = (1 - Math.abs(2 * l - 1)) * s;
    const x = c * (1 - Math.abs((h / 60) % 2 - 1));
    const m = l - c / 2;

    let r = 0, g = 0, b = 0;
    if (h < 60) { r = c; g = x; }
    else if (h < 120) { r = x; g = c; }
    else if (h < 180) { g = c; b = x; }
    else if (h < 240) { g = x; b = c; }
    else if (h < 300) { r = x; b = c; }
    else { r = c; b = x; }

    return {
      r: Math.floor((r + m) * 255),
      g: Math.floor((g + m) * 255),
      b: Math.floor((b + m) * 255),
    };
  }

  private viridis(t: number): { r: number; g: number; b: number } {
    // Simplified viridis colormap
    return {
      r: Math.floor((0.267004 + t * (0.993248 - 0.267004)) * 255),
      g: Math.floor((0.004874 + t * (0.906157 - 0.004874)) * 255),
      b: Math.floor((0.329415 + t * (0.143936 - 0.329415 + 0.5)) * 255),
    };
  }

  private magma(t: number): { r: number; g: number; b: number } {
    return {
      r: Math.floor((0.001462 + t * (0.987053 - 0.001462)) * 255),
      g: Math.floor((0.000466 + t * (0.991438 - 0.000466)) * 255),
      b: Math.floor((0.013866 + t * (0.749504 - 0.013866)) * 255),
    };
  }

  private plasma(t: number): { r: number; g: number; b: number } {
    return {
      r: Math.floor((0.050383 + t * (0.940015 - 0.050383)) * 255),
      g: Math.floor((0.029803 + t * (0.975158 - 0.029803)) * 255),
      b: Math.floor((0.527975 + t * (0.131326 - 0.527975 + 0.5)) * 255),
    };
  }

  clear(): void {
    const { width, height } = this.canvas;
    this.ctx.fillStyle = this.style.backgroundColor;
    this.ctx.fillRect(0, 0, width, height);
    this.imageData = this.ctx.createImageData(width, height);
  }

  resize(): void {
    this.imageData = this.ctx.createImageData(this.canvas.width, this.canvas.height);
  }
}
