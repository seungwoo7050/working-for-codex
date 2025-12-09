/**
 * Multi-Pass Renderer - 다중 패스 렌더링
 * @version 3.1.3
 */

import { ShaderProgram } from './ShaderProgram';
import { Framebuffer } from './Framebuffer';

export interface RenderPass {
  name: string;
  shader: ShaderProgram;
  inputs: WebGLTexture[];
  output: Framebuffer | null; // null = 화면 출력
  setup?: (gl: WebGL2RenderingContext) => void;
  uniforms?: Record<string, unknown>;
}

export class MultiPassRenderer {
  private gl: WebGL2RenderingContext;
  private passes: RenderPass[] = [];
  private quadVAO: WebGLVertexArrayObject;
  private quadVBO: WebGLBuffer;
  private viewportWidth: number;
  private viewportHeight: number;

  constructor(gl: WebGL2RenderingContext, viewportWidth: number, viewportHeight: number) {
    this.gl = gl;
    this.viewportWidth = viewportWidth;
    this.viewportHeight = viewportHeight;
    
    const { vao, vbo } = this.createFullscreenQuad();
    this.quadVAO = vao;
    this.quadVBO = vbo;
  }

  private createFullscreenQuad(): { vao: WebGLVertexArrayObject; vbo: WebGLBuffer } {
    const gl = this.gl;
    
    const vao = gl.createVertexArray();
    if (!vao) throw new Error('Failed to create VAO');
    
    const vbo = gl.createBuffer();
    if (!vbo) throw new Error('Failed to create VBO');

    // Fullscreen quad vertices (position + texcoord)
    const vertices = new Float32Array([
      // Position    // TexCoord
      -1, -1,        0, 0,
       1, -1,        1, 0,
      -1,  1,        0, 1,
      -1,  1,        0, 1,
       1, -1,        1, 0,
       1,  1,        1, 1,
    ]);

    gl.bindVertexArray(vao);
    gl.bindBuffer(gl.ARRAY_BUFFER, vbo);
    gl.bufferData(gl.ARRAY_BUFFER, vertices, gl.STATIC_DRAW);

    // Position attribute
    gl.enableVertexAttribArray(0);
    gl.vertexAttribPointer(0, 2, gl.FLOAT, false, 16, 0);

    // TexCoord attribute
    gl.enableVertexAttribArray(1);
    gl.vertexAttribPointer(1, 2, gl.FLOAT, false, 16, 8);

    gl.bindVertexArray(null);

    return { vao, vbo };
  }

  addPass(pass: RenderPass): void {
    this.passes.push(pass);
  }

  removePass(name: string): void {
    this.passes = this.passes.filter(p => p.name !== name);
  }

  clearPasses(): void {
    this.passes = [];
  }

  render(): void {
    const gl = this.gl;

    for (const pass of this.passes) {
      // Bind output
      if (pass.output) {
        pass.output.bind();
      } else {
        gl.bindFramebuffer(gl.FRAMEBUFFER, null);
        gl.viewport(0, 0, this.viewportWidth, this.viewportHeight);
      }

      // Clear
      gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);

      // Use shader
      pass.shader.use();

      // Custom setup
      if (pass.setup) {
        pass.setup(gl);
      }

      // Set uniforms
      if (pass.uniforms) {
        for (const [name, value] of Object.entries(pass.uniforms)) {
          this.setUniform(pass.shader, name, value);
        }
      }

      // Bind input textures
      pass.inputs.forEach((texture, index) => {
        gl.activeTexture(gl.TEXTURE0 + index);
        gl.bindTexture(gl.TEXTURE_2D, texture);
        pass.shader.setUniform1i(`uTexture${index}`, index);
      });

      // Draw fullscreen quad
      gl.bindVertexArray(this.quadVAO);
      gl.drawArrays(gl.TRIANGLES, 0, 6);
    }

    gl.bindVertexArray(null);
  }

  private setUniform(shader: ShaderProgram, name: string, value: unknown): void {
    if (typeof value === 'number') {
      if (Number.isInteger(value)) {
        shader.setUniform1i(name, value);
      } else {
        shader.setUniform1f(name, value);
      }
    } else if (Array.isArray(value)) {
      switch (value.length) {
        case 2: shader.setUniform2f(name, value[0], value[1]); break;
        case 3: shader.setUniform3f(name, value[0], value[1], value[2]); break;
        case 4: shader.setUniform4f(name, value[0], value[1], value[2], value[3]); break;
      }
    }
  }

  setViewportSize(width: number, height: number): void {
    this.viewportWidth = width;
    this.viewportHeight = height;
  }

  dispose(): void {
    this.gl.deleteVertexArray(this.quadVAO);
    this.gl.deleteBuffer(this.quadVBO);
  }
}
