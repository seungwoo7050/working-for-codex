/**
 * WebGL Audio Visualizer - WebGL 기반 시각화
 * @version 3.2.3
 */

export interface WebGLVisualizerOptions {
  particleCount?: number;
  particleSize?: number;
  colorPalette?: number[][];
}

export class WebGLAudioVisualizer {
  private canvas: HTMLCanvasElement;
  private gl: WebGL2RenderingContext;
  private program: WebGLProgram | null = null;
  private vao: WebGLVertexArrayObject | null = null;
  private frequencyTexture: WebGLTexture | null = null;
  private animationId: number | null = null;
  private isRunning: boolean = false;
  private frequencyData: Uint8Array;
  private options: Required<WebGLVisualizerOptions>;

  constructor(
    canvas: HTMLCanvasElement,
    frequencyBinCount: number,
    options: WebGLVisualizerOptions = {}
  ) {
    this.canvas = canvas;
    const gl = canvas.getContext('webgl2');
    if (!gl) {
      throw new Error('WebGL2 not supported');
    }
    this.gl = gl;

    this.options = {
      particleCount: options.particleCount ?? 5000,
      particleSize: options.particleSize ?? 3,
      colorPalette: options.colorPalette ?? [
        [0, 0.5, 1],
        [0, 1, 0.5],
        [1, 0.5, 0],
      ],
    };

    this.frequencyData = new Uint8Array(frequencyBinCount);
    this.initGL();
  }

  private initGL(): void {
    const gl = this.gl;

    // Vertex shader
    const vsSource = `#version 300 es
      in vec2 aPosition;
      uniform sampler2D uFrequency;
      uniform float uTime;
      uniform vec2 uResolution;
      out float vEnergy;
      out vec2 vUV;

      void main() {
        vec2 pos = aPosition;
        
        // Sample frequency data
        float freqIndex = (pos.x + 1.0) * 0.5;
        vEnergy = texture(uFrequency, vec2(freqIndex, 0.5)).r;
        
        // Animate based on energy
        pos.y += sin(uTime * 2.0 + pos.x * 3.0) * vEnergy * 0.3;
        pos.x += cos(uTime * 1.5 + pos.y * 2.0) * vEnergy * 0.2;
        
        gl_Position = vec4(pos, 0.0, 1.0);
        gl_PointSize = ${this.options.particleSize}.0 + vEnergy * 5.0;
        vUV = pos * 0.5 + 0.5;
      }
    `;

    // Fragment shader
    const fsSource = `#version 300 es
      precision highp float;
      in float vEnergy;
      in vec2 vUV;
      out vec4 fragColor;

      void main() {
        // Distance from center of point
        vec2 coord = gl_PointCoord - vec2(0.5);
        float dist = length(coord);
        if (dist > 0.5) discard;
        
        // Color based on energy
        vec3 color = mix(
          vec3(0.0, 0.5, 1.0),
          vec3(1.0, 0.3, 0.0),
          vEnergy
        );
        
        float alpha = (1.0 - dist * 2.0) * (0.5 + vEnergy * 0.5);
        fragColor = vec4(color, alpha);
      }
    `;

    // Compile shaders
    const vs = this.compileShader(gl.VERTEX_SHADER, vsSource);
    const fs = this.compileShader(gl.FRAGMENT_SHADER, fsSource);
    if (!vs || !fs) return;

    // Create program
    const program = gl.createProgram()!;
    gl.attachShader(program, vs);
    gl.attachShader(program, fs);
    gl.linkProgram(program);

    if (!gl.getProgramParameter(program, gl.LINK_STATUS)) {
      console.error('Program link error:', gl.getProgramInfoLog(program));
      return;
    }

    this.program = program;
    gl.deleteShader(vs);
    gl.deleteShader(fs);

    // Create particle positions
    const positions = new Float32Array(this.options.particleCount * 2);
    for (let i = 0; i < this.options.particleCount; i++) {
      positions[i * 2] = (Math.random() * 2 - 1);
      positions[i * 2 + 1] = (Math.random() * 2 - 1);
    }

    // Create VAO and VBO
    this.vao = gl.createVertexArray();
    gl.bindVertexArray(this.vao);

    const vbo = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER, vbo);
    gl.bufferData(gl.ARRAY_BUFFER, positions, gl.STATIC_DRAW);

    const posLoc = gl.getAttribLocation(program, 'aPosition');
    gl.enableVertexAttribArray(posLoc);
    gl.vertexAttribPointer(posLoc, 2, gl.FLOAT, false, 0, 0);

    gl.bindVertexArray(null);

    // Create frequency texture
    this.frequencyTexture = gl.createTexture();
    gl.bindTexture(gl.TEXTURE_2D, this.frequencyTexture);
    gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.LINEAR);
    gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MAG_FILTER, gl.LINEAR);
    gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_S, gl.CLAMP_TO_EDGE);
    gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_T, gl.CLAMP_TO_EDGE);

    // Enable blending
    gl.enable(gl.BLEND);
    gl.blendFunc(gl.SRC_ALPHA, gl.ONE_MINUS_SRC_ALPHA);
  }

  private compileShader(type: number, source: string): WebGLShader | null {
    const gl = this.gl;
    const shader = gl.createShader(type);
    if (!shader) return null;

    gl.shaderSource(shader, source);
    gl.compileShader(shader);

    if (!gl.getShaderParameter(shader, gl.COMPILE_STATUS)) {
      console.error('Shader compile error:', gl.getShaderInfoLog(shader));
      gl.deleteShader(shader);
      return null;
    }

    return shader;
  }

  updateFrequencyData(data: Uint8Array): void {
    this.frequencyData.set(data);

    const gl = this.gl;
    gl.bindTexture(gl.TEXTURE_2D, this.frequencyTexture);
    gl.texImage2D(
      gl.TEXTURE_2D,
      0,
      gl.R8,
      data.length,
      1,
      0,
      gl.RED,
      gl.UNSIGNED_BYTE,
      data
    );
  }

  private startTime = 0;

  start(): void {
    if (this.isRunning) return;
    this.isRunning = true;
    this.startTime = performance.now();
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
    if (!this.isRunning || !this.program) return;

    const gl = this.gl;
    const time = (performance.now() - this.startTime) / 1000;

    gl.viewport(0, 0, this.canvas.width, this.canvas.height);
    gl.clearColor(0, 0, 0, 1);
    gl.clear(gl.COLOR_BUFFER_BIT);

    gl.useProgram(this.program);

    // Set uniforms
    const timeLoc = gl.getUniformLocation(this.program, 'uTime');
    const resLoc = gl.getUniformLocation(this.program, 'uResolution');
    const freqLoc = gl.getUniformLocation(this.program, 'uFrequency');

    gl.uniform1f(timeLoc, time);
    gl.uniform2f(resLoc, this.canvas.width, this.canvas.height);

    gl.activeTexture(gl.TEXTURE0);
    gl.bindTexture(gl.TEXTURE_2D, this.frequencyTexture);
    gl.uniform1i(freqLoc, 0);

    // Draw
    gl.bindVertexArray(this.vao);
    gl.drawArrays(gl.POINTS, 0, this.options.particleCount);

    this.animationId = requestAnimationFrame(this.render);
  };

  dispose(): void {
    this.stop();
    
    const gl = this.gl;
    if (this.program) gl.deleteProgram(this.program);
    if (this.vao) gl.deleteVertexArray(this.vao);
    if (this.frequencyTexture) gl.deleteTexture(this.frequencyTexture);
  }
}
