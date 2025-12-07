/**
 * Uniform Binder - 유니폼 변수 바인딩
 * @version 3.1.1
 */

export class UniformBinder {
  private gl: WebGL2RenderingContext;

  constructor(gl: WebGL2RenderingContext) {
    this.gl = gl;
  }

  // Integer uniforms
  setInt(location: WebGLUniformLocation, value: number): void {
    this.gl.uniform1i(location, value);
  }

  setInt2(location: WebGLUniformLocation, x: number, y: number): void {
    this.gl.uniform2i(location, x, y);
  }

  setInt3(location: WebGLUniformLocation, x: number, y: number, z: number): void {
    this.gl.uniform3i(location, x, y, z);
  }

  setInt4(location: WebGLUniformLocation, x: number, y: number, z: number, w: number): void {
    this.gl.uniform4i(location, x, y, z, w);
  }

  // Float uniforms
  setFloat(location: WebGLUniformLocation, value: number): void {
    this.gl.uniform1f(location, value);
  }

  setVec2(location: WebGLUniformLocation, x: number, y: number): void {
    this.gl.uniform2f(location, x, y);
  }

  setVec3(location: WebGLUniformLocation, x: number, y: number, z: number): void {
    this.gl.uniform3f(location, x, y, z);
  }

  setVec4(location: WebGLUniformLocation, x: number, y: number, z: number, w: number): void {
    this.gl.uniform4f(location, x, y, z, w);
  }

  // Array uniforms
  setFloatArray(location: WebGLUniformLocation, values: Float32Array | number[]): void {
    this.gl.uniform1fv(location, values);
  }

  setVec2Array(location: WebGLUniformLocation, values: Float32Array | number[]): void {
    this.gl.uniform2fv(location, values);
  }

  setVec3Array(location: WebGLUniformLocation, values: Float32Array | number[]): void {
    this.gl.uniform3fv(location, values);
  }

  setVec4Array(location: WebGLUniformLocation, values: Float32Array | number[]): void {
    this.gl.uniform4fv(location, values);
  }

  // Matrix uniforms
  setMat2(location: WebGLUniformLocation, matrix: Float32Array): void {
    this.gl.uniformMatrix2fv(location, false, matrix);
  }

  setMat3(location: WebGLUniformLocation, matrix: Float32Array): void {
    this.gl.uniformMatrix3fv(location, false, matrix);
  }

  setMat4(location: WebGLUniformLocation, matrix: Float32Array): void {
    this.gl.uniformMatrix4fv(location, false, matrix);
  }

  // Texture binding
  setTexture(location: WebGLUniformLocation, texture: WebGLTexture, unit: number): void {
    this.gl.activeTexture(this.gl.TEXTURE0 + unit);
    this.gl.bindTexture(this.gl.TEXTURE_2D, texture);
    this.gl.uniform1i(location, unit);
  }

  setCubeTexture(location: WebGLUniformLocation, texture: WebGLTexture, unit: number): void {
    this.gl.activeTexture(this.gl.TEXTURE0 + unit);
    this.gl.bindTexture(this.gl.TEXTURE_CUBE_MAP, texture);
    this.gl.uniform1i(location, unit);
  }

  // Boolean (as int)
  setBool(location: WebGLUniformLocation, value: boolean): void {
    this.gl.uniform1i(location, value ? 1 : 0);
  }

  // Color (normalized RGBA)
  setColor(location: WebGLUniformLocation, r: number, g: number, b: number, a = 1.0): void {
    this.gl.uniform4f(location, r, g, b, a);
  }

  // Hex color
  setColorHex(location: WebGLUniformLocation, hex: string, alpha = 1.0): void {
    const r = parseInt(hex.slice(1, 3), 16) / 255;
    const g = parseInt(hex.slice(3, 5), 16) / 255;
    const b = parseInt(hex.slice(5, 7), 16) / 255;
    this.gl.uniform4f(location, r, g, b, alpha);
  }
}
