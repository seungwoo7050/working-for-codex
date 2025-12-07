/**
 * Post Processor - 포스트 프로세싱 효과
 * @version 3.1.3
 */

import { ShaderProgram } from './ShaderProgram';
import { Framebuffer } from './Framebuffer';
import { ShaderCompiler } from './ShaderCompiler';

export type PostEffect = 'blur' | 'brightness' | 'contrast' | 'saturation' | 'vignette';

export class PostProcessor {
  private gl: WebGL2RenderingContext;
  private shaders: Map<PostEffect, ShaderProgram> = new Map();
  private tempFramebuffers: Framebuffer[] = [];
  private quadVAO: WebGLVertexArrayObject;
  private quadVBO: WebGLBuffer;

  constructor(gl: WebGL2RenderingContext) {
    this.gl = gl;
    const { vao, vbo } = this.createQuad();
    this.quadVAO = vao;
    this.quadVBO = vbo;
    this.initShaders();
  }

  private createQuad(): { vao: WebGLVertexArrayObject; vbo: WebGLBuffer } {
    const gl = this.gl;
    
    const vao = gl.createVertexArray()!;
    const vbo = gl.createBuffer()!;

    const vertices = new Float32Array([
      -1, -1, 0, 0,
       1, -1, 1, 0,
      -1,  1, 0, 1,
      -1,  1, 0, 1,
       1, -1, 1, 0,
       1,  1, 1, 1,
    ]);

    gl.bindVertexArray(vao);
    gl.bindBuffer(gl.ARRAY_BUFFER, vbo);
    gl.bufferData(gl.ARRAY_BUFFER, vertices, gl.STATIC_DRAW);
    gl.enableVertexAttribArray(0);
    gl.vertexAttribPointer(0, 2, gl.FLOAT, false, 16, 0);
    gl.enableVertexAttribArray(1);
    gl.vertexAttribPointer(1, 2, gl.FLOAT, false, 16, 8);
    gl.bindVertexArray(null);

    return { vao, vbo };
  }

  private initShaders(): void {
    const vertexShader = ShaderCompiler.getDefaultVertexShader();

    // Blur shader
    this.shaders.set('blur', new ShaderProgram(this.gl, {
      vertexSource: vertexShader,
      fragmentSource: `#version 300 es
        precision highp float;
        uniform sampler2D uTexture;
        uniform vec2 uDirection;
        uniform vec2 uResolution;
        in vec2 vTexCoord;
        out vec4 fragColor;
        
        void main() {
          vec2 texelSize = 1.0 / uResolution;
          vec4 result = vec4(0.0);
          float weights[5] = float[](0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);
          
          result += texture(uTexture, vTexCoord) * weights[0];
          for (int i = 1; i < 5; i++) {
            vec2 offset = uDirection * texelSize * float(i);
            result += texture(uTexture, vTexCoord + offset) * weights[i];
            result += texture(uTexture, vTexCoord - offset) * weights[i];
          }
          
          fragColor = result;
        }
      `,
    }));

    // Brightness/Contrast shader
    this.shaders.set('brightness', new ShaderProgram(this.gl, {
      vertexSource: vertexShader,
      fragmentSource: `#version 300 es
        precision highp float;
        uniform sampler2D uTexture;
        uniform float uBrightness;
        uniform float uContrast;
        in vec2 vTexCoord;
        out vec4 fragColor;
        
        void main() {
          vec4 color = texture(uTexture, vTexCoord);
          color.rgb += uBrightness;
          color.rgb = (color.rgb - 0.5) * uContrast + 0.5;
          fragColor = color;
        }
      `,
    }));

    // Vignette shader
    this.shaders.set('vignette', new ShaderProgram(this.gl, {
      vertexSource: vertexShader,
      fragmentSource: `#version 300 es
        precision highp float;
        uniform sampler2D uTexture;
        uniform float uIntensity;
        uniform float uSmoothness;
        in vec2 vTexCoord;
        out vec4 fragColor;
        
        void main() {
          vec4 color = texture(uTexture, vTexCoord);
          vec2 center = vTexCoord - 0.5;
          float dist = length(center);
          float vignette = smoothstep(0.5, 0.5 - uSmoothness, dist * uIntensity);
          fragColor = vec4(color.rgb * vignette, color.a);
        }
      `,
    }));
  }

  applyBlur(
    input: WebGLTexture,
    output: Framebuffer | null,
    iterations: number = 2,
    width: number,
    height: number
  ): void {
    const shader = this.shaders.get('blur')!;
    shader.use();
    shader.setUniform2f('uResolution', width, height);

    // Ensure we have temp framebuffers
    if (this.tempFramebuffers.length < 2) {
      this.tempFramebuffers.push(
        new Framebuffer(this.gl, { width, height }),
        new Framebuffer(this.gl, { width, height })
      );
    }

    let currentInput = input;
    
    for (let i = 0; i < iterations * 2; i++) {
      const isHorizontal = i % 2 === 0;
      const isLast = i === iterations * 2 - 1;
      const target = isLast ? output : this.tempFramebuffers[i % 2];

      if (target) {
        target.bind();
      } else {
        this.gl.bindFramebuffer(this.gl.FRAMEBUFFER, null);
      }

      shader.setUniform2f('uDirection', isHorizontal ? 1 : 0, isHorizontal ? 0 : 1);

      this.gl.activeTexture(this.gl.TEXTURE0);
      this.gl.bindTexture(this.gl.TEXTURE_2D, currentInput);
      shader.setUniform1i('uTexture', 0);

      this.gl.bindVertexArray(this.quadVAO);
      this.gl.drawArrays(this.gl.TRIANGLES, 0, 6);

      if (!isLast) {
        currentInput = this.tempFramebuffers[i % 2].getColorTexture();
      }
    }
  }

  applyBrightnessContrast(
    input: WebGLTexture,
    output: Framebuffer | null,
    brightness: number = 0,
    contrast: number = 1
  ): void {
    const shader = this.shaders.get('brightness')!;
    
    if (output) {
      output.bind();
    } else {
      this.gl.bindFramebuffer(this.gl.FRAMEBUFFER, null);
    }

    shader.use();
    shader.setUniform1f('uBrightness', brightness);
    shader.setUniform1f('uContrast', contrast);

    this.gl.activeTexture(this.gl.TEXTURE0);
    this.gl.bindTexture(this.gl.TEXTURE_2D, input);
    shader.setUniform1i('uTexture', 0);

    this.gl.bindVertexArray(this.quadVAO);
    this.gl.drawArrays(this.gl.TRIANGLES, 0, 6);
  }

  applyVignette(
    input: WebGLTexture,
    output: Framebuffer | null,
    intensity: number = 1.5,
    smoothness: number = 0.4
  ): void {
    const shader = this.shaders.get('vignette')!;
    
    if (output) {
      output.bind();
    } else {
      this.gl.bindFramebuffer(this.gl.FRAMEBUFFER, null);
    }

    shader.use();
    shader.setUniform1f('uIntensity', intensity);
    shader.setUniform1f('uSmoothness', smoothness);

    this.gl.activeTexture(this.gl.TEXTURE0);
    this.gl.bindTexture(this.gl.TEXTURE_2D, input);
    shader.setUniform1i('uTexture', 0);

    this.gl.bindVertexArray(this.quadVAO);
    this.gl.drawArrays(this.gl.TRIANGLES, 0, 6);
  }

  dispose(): void {
    this.shaders.forEach(shader => shader.dispose());
    this.shaders.clear();
    this.tempFramebuffers.forEach(fb => fb.dispose());
    this.tempFramebuffers = [];
    this.gl.deleteVertexArray(this.quadVAO);
    this.gl.deleteBuffer(this.quadVBO);
  }
}
