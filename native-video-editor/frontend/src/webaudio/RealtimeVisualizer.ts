/**
 * Realtime Visualizer - 실시간 오디오 시각화
 * @version 3.2.3
 */

import { FFTAnalyzer } from './FFTAnalyzer';

export type VisualizationType = 'bars' | 'line' | 'circle' | 'particles';

export interface VisualizerStyle {
  type?: VisualizationType;
  barColor?: string;
  barGradient?: string[];
  barWidth?: number;
  barGap?: number;
  lineColor?: string;
  lineWidth?: number;
  circleRadius?: number;
  particleCount?: number;
  backgroundColor?: string;
  mirror?: boolean;
}

export class RealtimeVisualizer {
  private canvas: HTMLCanvasElement;
  private ctx: CanvasRenderingContext2D;
  private analyzer: FFTAnalyzer;
  private style: Required<VisualizerStyle>;
  private animationId: number | null = null;
  private isRunning: boolean = false;

  constructor(
    canvas: HTMLCanvasElement,
    analyzer: FFTAnalyzer,
    style: VisualizerStyle = {}
  ) {
    this.canvas = canvas;
    const ctx = canvas.getContext('2d');
    if (!ctx) {
      throw new Error('Failed to get 2D context');
    }
    this.ctx = ctx;
    this.analyzer = analyzer;

    this.style = {
      type: style.type ?? 'bars',
      barColor: style.barColor ?? '#00ff00',
      barGradient: style.barGradient ?? [],
      barWidth: style.barWidth ?? 4,
      barGap: style.barGap ?? 2,
      lineColor: style.lineColor ?? '#00ff00',
      lineWidth: style.lineWidth ?? 2,
      circleRadius: style.circleRadius ?? 100,
      particleCount: style.particleCount ?? 100,
      backgroundColor: style.backgroundColor ?? '#000000',
      mirror: style.mirror ?? false,
    };
  }

  setStyle(style: Partial<VisualizerStyle>): void {
    Object.assign(this.style, style);
  }

  start(): void {
    if (this.isRunning) return;
    this.isRunning = true;
    this.render();
  }

  stop(): void {
    this.isRunning = false;
    if (this.animationId !== null) {
      cancelAnimationFrame(this.animationId);
      this.animationId = null;
    }
  }

  private render = (): void => {
    if (!this.isRunning) return;

    this.clear();

    switch (this.style.type) {
      case 'bars':
        this.renderBars();
        break;
      case 'line':
        this.renderLine();
        break;
      case 'circle':
        this.renderCircle();
        break;
      case 'particles':
        this.renderParticles();
        break;
    }

    this.animationId = requestAnimationFrame(this.render);
  };

  private clear(): void {
    this.ctx.fillStyle = this.style.backgroundColor;
    this.ctx.fillRect(0, 0, this.canvas.width, this.canvas.height);
  }

  private renderBars(): void {
    const frequencyData = this.analyzer.getFrequencyData();
    const { width, height } = this.canvas;
    const ctx = this.ctx;

    const barWidth = this.style.barWidth;
    const barGap = this.style.barGap;
    const barCount = Math.floor(width / (barWidth + barGap));
    const step = Math.floor(frequencyData.length / barCount);

    // Create gradient if specified
    let fillStyle: string | CanvasGradient = this.style.barColor;
    if (this.style.barGradient.length >= 2) {
      const gradient = ctx.createLinearGradient(0, height, 0, 0);
      this.style.barGradient.forEach((color, i) => {
        gradient.addColorStop(i / (this.style.barGradient.length - 1), color);
      });
      fillStyle = gradient;
    }

    ctx.fillStyle = fillStyle;

    for (let i = 0; i < barCount; i++) {
      const value = frequencyData[i * step] / 255;
      const barHeight = value * height;
      const x = i * (barWidth + barGap);
      const y = height - barHeight;

      ctx.fillRect(x, y, barWidth, barHeight);

      if (this.style.mirror) {
        ctx.fillRect(x, 0, barWidth, barHeight);
      }
    }
  }

  private renderLine(): void {
    const frequencyData = this.analyzer.getFrequencyData();
    const { width, height } = this.canvas;
    const ctx = this.ctx;

    ctx.strokeStyle = this.style.lineColor;
    ctx.lineWidth = this.style.lineWidth;
    ctx.beginPath();

    const sliceWidth = width / frequencyData.length;

    for (let i = 0; i < frequencyData.length; i++) {
      const value = frequencyData[i] / 255;
      const x = i * sliceWidth;
      const y = height - value * height;

      if (i === 0) {
        ctx.moveTo(x, y);
      } else {
        ctx.lineTo(x, y);
      }
    }

    ctx.stroke();
  }

  private renderCircle(): void {
    const frequencyData = this.analyzer.getFrequencyData();
    const { width, height } = this.canvas;
    const ctx = this.ctx;

    const centerX = width / 2;
    const centerY = height / 2;
    const radius = this.style.circleRadius;
    const bars = 64;
    const step = Math.floor(frequencyData.length / bars);

    ctx.strokeStyle = this.style.barColor;
    ctx.lineWidth = this.style.barWidth;

    for (let i = 0; i < bars; i++) {
      const value = frequencyData[i * step] / 255;
      const angle = (i / bars) * Math.PI * 2 - Math.PI / 2;
      const barLength = value * radius;

      const x1 = centerX + Math.cos(angle) * radius;
      const y1 = centerY + Math.sin(angle) * radius;
      const x2 = centerX + Math.cos(angle) * (radius + barLength);
      const y2 = centerY + Math.sin(angle) * (radius + barLength);

      ctx.beginPath();
      ctx.moveTo(x1, y1);
      ctx.lineTo(x2, y2);
      ctx.stroke();
    }
  }

  private particles: Array<{
    x: number;
    y: number;
    vx: number;
    vy: number;
    size: number;
  }> = [];

  private renderParticles(): void {
    const frequencyData = this.analyzer.getFrequencyData();
    const { width, height } = this.canvas;
    const ctx = this.ctx;

    // Initialize particles if needed
    if (this.particles.length === 0) {
      for (let i = 0; i < this.style.particleCount; i++) {
        this.particles.push({
          x: Math.random() * width,
          y: Math.random() * height,
          vx: (Math.random() - 0.5) * 2,
          vy: (Math.random() - 0.5) * 2,
          size: Math.random() * 3 + 1,
        });
      }
    }

    // Get average energy
    let energy = 0;
    for (let i = 0; i < frequencyData.length; i++) {
      energy += frequencyData[i];
    }
    energy = energy / frequencyData.length / 255;

    ctx.fillStyle = this.style.barColor;

    for (const particle of this.particles) {
      // Update position
      particle.x += particle.vx * (1 + energy * 3);
      particle.y += particle.vy * (1 + energy * 3);

      // Wrap around
      if (particle.x < 0) particle.x = width;
      if (particle.x > width) particle.x = 0;
      if (particle.y < 0) particle.y = height;
      if (particle.y > height) particle.y = 0;

      // Draw
      ctx.beginPath();
      ctx.arc(particle.x, particle.y, particle.size * (1 + energy), 0, Math.PI * 2);
      ctx.fill();
    }
  }

  dispose(): void {
    this.stop();
    this.particles = [];
  }
}
