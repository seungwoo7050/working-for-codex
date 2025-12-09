/**
 * Waveform Renderer - 파형 렌더링
 * @version 3.2.3
 */

export interface WaveformStyle {
  lineColor?: string;
  lineWidth?: number;
  fillColor?: string;
  backgroundColor?: string;
  centerLine?: boolean;
  centerLineColor?: string;
}

export class WaveformRenderer {
  private canvas: HTMLCanvasElement;
  private ctx: CanvasRenderingContext2D;
  private style: Required<WaveformStyle>;

  constructor(canvas: HTMLCanvasElement, style: WaveformStyle = {}) {
    this.canvas = canvas;
    const ctx = canvas.getContext('2d');
    if (!ctx) {
      throw new Error('Failed to get 2D context');
    }
    this.ctx = ctx;

    this.style = {
      lineColor: style.lineColor ?? '#00ff00',
      lineWidth: style.lineWidth ?? 2,
      fillColor: style.fillColor ?? 'rgba(0, 255, 0, 0.3)',
      backgroundColor: style.backgroundColor ?? '#000000',
      centerLine: style.centerLine ?? true,
      centerLineColor: style.centerLineColor ?? 'rgba(255, 255, 255, 0.3)',
    };
  }

  setStyle(style: Partial<WaveformStyle>): void {
    Object.assign(this.style, style);
  }

  /**
   * 실시간 오디오 데이터 렌더링
   */
  renderRealtime(timeDomainData: Uint8Array): void {
    const { width, height } = this.canvas;
    const ctx = this.ctx;

    // Clear
    ctx.fillStyle = this.style.backgroundColor;
    ctx.fillRect(0, 0, width, height);

    // Center line
    if (this.style.centerLine) {
      ctx.strokeStyle = this.style.centerLineColor;
      ctx.lineWidth = 1;
      ctx.beginPath();
      ctx.moveTo(0, height / 2);
      ctx.lineTo(width, height / 2);
      ctx.stroke();
    }

    // Waveform
    ctx.strokeStyle = this.style.lineColor;
    ctx.lineWidth = this.style.lineWidth;
    ctx.beginPath();

    const sliceWidth = width / timeDomainData.length;
    let x = 0;

    for (let i = 0; i < timeDomainData.length; i++) {
      const v = timeDomainData[i] / 128.0;
      const y = (v * height) / 2;

      if (i === 0) {
        ctx.moveTo(x, y);
      } else {
        ctx.lineTo(x, y);
      }

      x += sliceWidth;
    }

    ctx.stroke();
  }

  /**
   * AudioBuffer 전체 파형 렌더링
   */
  renderBuffer(buffer: AudioBuffer, channel: number = 0): void {
    const { width, height } = this.canvas;
    const ctx = this.ctx;
    const data = buffer.getChannelData(channel);

    // Clear
    ctx.fillStyle = this.style.backgroundColor;
    ctx.fillRect(0, 0, width, height);

    // Center line
    if (this.style.centerLine) {
      ctx.strokeStyle = this.style.centerLineColor;
      ctx.lineWidth = 1;
      ctx.beginPath();
      ctx.moveTo(0, height / 2);
      ctx.lineTo(width, height / 2);
      ctx.stroke();
    }

    // Calculate samples per pixel
    const samplesPerPixel = Math.ceil(data.length / width);
    const centerY = height / 2;

    // Draw waveform as min/max bars
    ctx.fillStyle = this.style.fillColor;
    ctx.strokeStyle = this.style.lineColor;
    ctx.lineWidth = 1;

    for (let x = 0; x < width; x++) {
      const start = x * samplesPerPixel;
      const end = Math.min(start + samplesPerPixel, data.length);

      let min = 1;
      let max = -1;

      for (let i = start; i < end; i++) {
        if (data[i] < min) min = data[i];
        if (data[i] > max) max = data[i];
      }

      const y1 = centerY - max * centerY;
      const y2 = centerY - min * centerY;

      ctx.fillRect(x, y1, 1, y2 - y1);
    }
  }

  /**
   * 축소된 파형 데이터 생성
   */
  static generateWaveformData(
    buffer: AudioBuffer,
    channel: number,
    pointCount: number
  ): { min: Float32Array; max: Float32Array } {
    const data = buffer.getChannelData(channel);
    const samplesPerPoint = Math.ceil(data.length / pointCount);

    const min = new Float32Array(pointCount);
    const max = new Float32Array(pointCount);

    for (let i = 0; i < pointCount; i++) {
      const start = i * samplesPerPoint;
      const end = Math.min(start + samplesPerPoint, data.length);

      let minVal = 1;
      let maxVal = -1;

      for (let j = start; j < end; j++) {
        if (data[j] < minVal) minVal = data[j];
        if (data[j] > maxVal) maxVal = data[j];
      }

      min[i] = minVal;
      max[i] = maxVal;
    }

    return { min, max };
  }

  /**
   * 축소된 데이터로 빠른 렌더링
   */
  renderWaveformData(
    data: { min: Float32Array; max: Float32Array },
    startIndex: number = 0,
    endIndex?: number
  ): void {
    const { width, height } = this.canvas;
    const ctx = this.ctx;

    endIndex = endIndex ?? data.min.length;
    const pointCount = endIndex - startIndex;

    // Clear
    ctx.fillStyle = this.style.backgroundColor;
    ctx.fillRect(0, 0, width, height);

    // Center line
    if (this.style.centerLine) {
      ctx.strokeStyle = this.style.centerLineColor;
      ctx.lineWidth = 1;
      ctx.beginPath();
      ctx.moveTo(0, height / 2);
      ctx.lineTo(width, height / 2);
      ctx.stroke();
    }

    // Draw
    ctx.fillStyle = this.style.fillColor;
    const centerY = height / 2;
    const pixelsPerPoint = width / pointCount;

    for (let i = 0; i < pointCount; i++) {
      const idx = startIndex + i;
      const x = i * pixelsPerPoint;
      const y1 = centerY - data.max[idx] * centerY;
      const y2 = centerY - data.min[idx] * centerY;

      ctx.fillRect(x, y1, Math.max(1, pixelsPerPoint), y2 - y1);
    }
  }

  clear(): void {
    this.ctx.fillStyle = this.style.backgroundColor;
    this.ctx.fillRect(0, 0, this.canvas.width, this.canvas.height);
  }
}
