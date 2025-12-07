/**
 * Batch Renderer - 배치 렌더링
 * @version 3.1.4
 */

import { ShaderProgram } from './ShaderProgram';
import { StateCache } from './StateCache';

export interface BatchItem {
  texture: WebGLTexture;
  x: number;
  y: number;
  width: number;
  height: number;
  u0?: number;
  v0?: number;
  u1?: number;
  v1?: number;
}

export class BatchRenderer {
  private gl: WebGL2RenderingContext;
  private stateCache: StateCache;
  private shader: ShaderProgram;
  
  private vao: WebGLVertexArrayObject;
  private vbo: WebGLBuffer;
  private ibo: WebGLBuffer;
  
  private maxBatchSize: number;
  private batchSize: number = 0;
  private vertices: Float32Array;
  private indices: Uint16Array;
  
  private currentTexture: WebGLTexture | null = null;
  private drawCalls: number = 0;
  private totalSprites: number = 0;

  // Per vertex: x, y, u, v (4 floats)
  private static readonly FLOATS_PER_VERTEX = 4;
  private static readonly VERTICES_PER_SPRITE = 4;
  private static readonly INDICES_PER_SPRITE = 6;

  constructor(
    gl: WebGL2RenderingContext,
    stateCache: StateCache,
    shader: ShaderProgram,
    maxBatchSize: number = 1000
  ) {
    this.gl = gl;
    this.stateCache = stateCache;
    this.shader = shader;
    this.maxBatchSize = maxBatchSize;
    
    // Allocate buffers
    const floatsPerSprite = BatchRenderer.FLOATS_PER_VERTEX * BatchRenderer.VERTICES_PER_SPRITE;
    this.vertices = new Float32Array(maxBatchSize * floatsPerSprite);
    this.indices = new Uint16Array(maxBatchSize * BatchRenderer.INDICES_PER_SPRITE);
    
    // Pre-generate indices
    for (let i = 0; i < maxBatchSize; i++) {
      const indexOffset = i * BatchRenderer.INDICES_PER_SPRITE;
      const vertexOffset = i * BatchRenderer.VERTICES_PER_SPRITE;
      
      this.indices[indexOffset + 0] = vertexOffset + 0;
      this.indices[indexOffset + 1] = vertexOffset + 1;
      this.indices[indexOffset + 2] = vertexOffset + 2;
      this.indices[indexOffset + 3] = vertexOffset + 2;
      this.indices[indexOffset + 4] = vertexOffset + 3;
      this.indices[indexOffset + 5] = vertexOffset + 0;
    }
    
    // Create VAO/VBO/IBO
    const vao = gl.createVertexArray();
    const vbo = gl.createBuffer();
    const ibo = gl.createBuffer();
    
    if (!vao || !vbo || !ibo) {
      throw new Error('Failed to create batch buffers');
    }
    
    this.vao = vao;
    this.vbo = vbo;
    this.ibo = ibo;
    
    gl.bindVertexArray(vao);
    
    // Setup VBO
    gl.bindBuffer(gl.ARRAY_BUFFER, vbo);
    gl.bufferData(gl.ARRAY_BUFFER, this.vertices.byteLength, gl.DYNAMIC_DRAW);
    
    // Position attribute
    gl.enableVertexAttribArray(0);
    gl.vertexAttribPointer(0, 2, gl.FLOAT, false, 16, 0);
    
    // TexCoord attribute
    gl.enableVertexAttribArray(1);
    gl.vertexAttribPointer(1, 2, gl.FLOAT, false, 16, 8);
    
    // Setup IBO
    gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, ibo);
    gl.bufferData(gl.ELEMENT_ARRAY_BUFFER, this.indices, gl.STATIC_DRAW);
    
    gl.bindVertexArray(null);
  }

  begin(): void {
    this.batchSize = 0;
    this.currentTexture = null;
    this.drawCalls = 0;
    this.totalSprites = 0;
    
    this.stateCache.useProgram(this.shader.getProgram());
    this.stateCache.bindVAO(this.vao);
  }

  draw(item: BatchItem): void {
    // Flush if texture changes or batch is full
    if (this.currentTexture !== item.texture) {
      this.flush();
      this.currentTexture = item.texture;
    }
    
    if (this.batchSize >= this.maxBatchSize) {
      this.flush();
    }
    
    // Add sprite to batch
    const floatsPerSprite = BatchRenderer.FLOATS_PER_VERTEX * BatchRenderer.VERTICES_PER_SPRITE;
    const offset = this.batchSize * floatsPerSprite;
    
    const x0 = item.x;
    const y0 = item.y;
    const x1 = item.x + item.width;
    const y1 = item.y + item.height;
    
    const u0 = item.u0 ?? 0;
    const v0 = item.v0 ?? 0;
    const u1 = item.u1 ?? 1;
    const v1 = item.v1 ?? 1;
    
    // Bottom-left
    this.vertices[offset + 0] = x0;
    this.vertices[offset + 1] = y0;
    this.vertices[offset + 2] = u0;
    this.vertices[offset + 3] = v0;
    
    // Bottom-right
    this.vertices[offset + 4] = x1;
    this.vertices[offset + 5] = y0;
    this.vertices[offset + 6] = u1;
    this.vertices[offset + 7] = v0;
    
    // Top-right
    this.vertices[offset + 8] = x1;
    this.vertices[offset + 9] = y1;
    this.vertices[offset + 10] = u1;
    this.vertices[offset + 11] = v1;
    
    // Top-left
    this.vertices[offset + 12] = x0;
    this.vertices[offset + 13] = y1;
    this.vertices[offset + 14] = u0;
    this.vertices[offset + 15] = v1;
    
    this.batchSize++;
    this.totalSprites++;
  }

  flush(): void {
    if (this.batchSize === 0) return;
    
    const gl = this.gl;
    
    // Bind texture
    if (this.currentTexture) {
      this.stateCache.activeTexture(0);
      this.stateCache.bindTexture(gl.TEXTURE_2D, this.currentTexture);
      this.shader.setUniform1i('uTexture', 0);
    }
    
    // Upload vertex data
    const floatsPerSprite = BatchRenderer.FLOATS_PER_VERTEX * BatchRenderer.VERTICES_PER_SPRITE;
    const dataToUpload = this.vertices.subarray(0, this.batchSize * floatsPerSprite);
    
    gl.bindBuffer(gl.ARRAY_BUFFER, this.vbo);
    gl.bufferSubData(gl.ARRAY_BUFFER, 0, dataToUpload);
    
    // Draw
    const indexCount = this.batchSize * BatchRenderer.INDICES_PER_SPRITE;
    gl.drawElements(gl.TRIANGLES, indexCount, gl.UNSIGNED_SHORT, 0);
    
    this.drawCalls++;
    this.batchSize = 0;
  }

  end(): void {
    this.flush();
  }

  getStats(): { drawCalls: number; sprites: number; batchEfficiency: number } {
    return {
      drawCalls: this.drawCalls,
      sprites: this.totalSprites,
      batchEfficiency: this.totalSprites > 0 ? this.totalSprites / this.drawCalls : 0,
    };
  }

  dispose(): void {
    this.gl.deleteVertexArray(this.vao);
    this.gl.deleteBuffer(this.vbo);
    this.gl.deleteBuffer(this.ibo);
  }
}
