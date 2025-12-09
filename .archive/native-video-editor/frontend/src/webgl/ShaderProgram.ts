// [FILE]
// - 목적: WebGL 셰이더 프로그램 생성 및 관리
// - 주요 역할: 버텍스/프래그먼트 셰이더 컴파일, 프로그램 링킹, 유니폼/어트리뷰트 관리
// - 관련 클론 가이드 단계: [CG-v3.1.1] WebGL 셰이더 시스템
// - 권장 읽는 순서: Order 1 (생성) → Order 2 (사용) → Order 3 (변수 접근)
//
// [LEARN] C 개발자를 위한 셰이더 이해:
// - 셰이더는 GPU에서 실행되는 작은 프로그램이다.
// - GLSL (OpenGL Shading Language)로 작성하며, C와 문법이 유사하다.
// - 버텍스 셰이더: 각 정점(vertex)의 위치 계산
// - 프래그먼트 셰이더: 각 픽셀의 색상 계산
// - C/OpenGL에서 glCreateShader(), glShaderSource(), glCompileShader()를 사용하는 것과 동일하다.

/**
 * Shader Program - 셰이더 프로그램 관리
 * @version 3.1.1
 */

import { ShaderCompiler } from './ShaderCompiler';

// [Order 1] 셰이더 프로그램 옵션
export interface ShaderProgramOptions {
  vertexSource: string;    // 버텍스 셰이더 GLSL 소스 코드
  fragmentSource: string;  // 프래그먼트 셰이더 GLSL 소스 코드
}

export class ShaderProgram {
  private gl: WebGL2RenderingContext;
  private program: WebGLProgram;
  // [LEARN] 유니폼/어트리뷰트 위치 캐싱
  // - GPU에 전달할 변수의 "위치"를 미리 조회하여 캐싱한다.
  // - C에서 glGetUniformLocation()을 매번 호출하는 오버헤드를 줄인다.
  private uniformLocations: Map<string, WebGLUniformLocation> = new Map();
  private attributeLocations: Map<string, number> = new Map();
  private isDisposed = false;

  constructor(gl: WebGL2RenderingContext, options: ShaderProgramOptions) {
    this.gl = gl;
    this.program = this.createProgram(options.vertexSource, options.fragmentSource);
  }

  // [Order 1-1] 셰이더 프로그램 생성
  // [LEARN] 셰이더 컴파일 및 링킹 과정
  // 1. 버텍스 셰이더 컴파일 (glCompileShader)
  // 2. 프래그먼트 셰이더 컴파일
  // 3. 프로그램 생성 (glCreateProgram)
  // 4. 셰이더들을 프로그램에 첨부 (glAttachShader)
  // 5. 프로그램 링킹 (glLinkProgram)
  // 6. 개별 셰이더 삭제 (링킹 후에는 불필요)
  private createProgram(vertexSource: string, fragmentSource: string): WebGLProgram {
    const compiler = new ShaderCompiler(this.gl);

    // Compile vertex shader
    const vertexResult = compiler.compileVertex(vertexSource);
    if (!vertexResult.success || !vertexResult.shader) {
      throw new Error(`Vertex shader compilation failed:\n${vertexResult.errors.join('\n')}`);
    }

    // Compile fragment shader
    const fragmentResult = compiler.compileFragment(fragmentSource);
    if (!fragmentResult.success || !fragmentResult.shader) {
      this.gl.deleteShader(vertexResult.shader);
      throw new Error(`Fragment shader compilation failed:\n${fragmentResult.errors.join('\n')}`);
    }

    // Create and link program
    const program = this.gl.createProgram();
    if (!program) {
      this.gl.deleteShader(vertexResult.shader);
      this.gl.deleteShader(fragmentResult.shader);
      throw new Error('Failed to create shader program');
    }

    this.gl.attachShader(program, vertexResult.shader);
    this.gl.attachShader(program, fragmentResult.shader);
    this.gl.linkProgram(program);

    // Check link status
    if (!this.gl.getProgramParameter(program, this.gl.LINK_STATUS)) {
      const log = this.gl.getProgramInfoLog(program) || 'Unknown link error';
      this.gl.deleteShader(vertexResult.shader);
      this.gl.deleteShader(fragmentResult.shader);
      this.gl.deleteProgram(program);
      throw new Error(`Shader program linking failed: ${log}`);
    }

    // Clean up shaders (they're now part of the program)
    this.gl.deleteShader(vertexResult.shader);
    this.gl.deleteShader(fragmentResult.shader);

    return program;
  }

  // [Order 2] 셰이더 프로그램 활성화
  // - C/OpenGL의 glUseProgram()과 동일하다.
  // - 이후 드로우 콜은 이 프로그램을 사용한다.
  use(): void {
    if (this.isDisposed) {
      throw new Error('ShaderProgram has been disposed');
    }
    this.gl.useProgram(this.program);
  }

  getProgram(): WebGLProgram {
    return this.program;
  }

  // [Order 3] 유니폼 변수 위치 조회
  // [LEARN] 유니폼(Uniform)이란?
  // - 셰이더에 전달하는 "상수" 값이다.
  // - 모든 정점/픽셀에서 동일한 값을 가진다.
  // - 예: 변환 행렬, 색상, 시간 등
  // - C/OpenGL의 glGetUniformLocation()과 동일하다.
  getUniformLocation(name: string): WebGLUniformLocation | null {
    if (this.uniformLocations.has(name)) {
      return this.uniformLocations.get(name)!;
    }

    const location = this.gl.getUniformLocation(this.program, name);
    if (location) {
      this.uniformLocations.set(name, location);
    }
    return location;
  }

  // [Order 3-1] 어트리뷰트 변수 위치 조회
  // [LEARN] 어트리뷰트(Attribute)란?
  // - 각 정점(vertex)마다 다른 값을 가지는 입력 데이터이다.
  // - 예: 정점 위치, 텍스처 좌표, 법선 벡터 등
  // - 버텍스 버퍼에서 데이터를 읽어온다.
  // - C/OpenGL의 glGetAttribLocation()과 동일하다.
  getAttributeLocation(name: string): number {
    if (this.attributeLocations.has(name)) {
      return this.attributeLocations.get(name)!;
    }

    const location = this.gl.getAttribLocation(this.program, name);
    this.attributeLocations.set(name, location);
    return location;
  }

  // Uniform setters
  setUniform1i(name: string, value: number): void {
    const location = this.getUniformLocation(name);
    if (location) {
      this.gl.uniform1i(location, value);
    }
  }

  setUniform1f(name: string, value: number): void {
    const location = this.getUniformLocation(name);
    if (location) {
      this.gl.uniform1f(location, value);
    }
  }

  setUniform2f(name: string, x: number, y: number): void {
    const location = this.getUniformLocation(name);
    if (location) {
      this.gl.uniform2f(location, x, y);
    }
  }

  setUniform3f(name: string, x: number, y: number, z: number): void {
    const location = this.getUniformLocation(name);
    if (location) {
      this.gl.uniform3f(location, x, y, z);
    }
  }

  setUniform4f(name: string, x: number, y: number, z: number, w: number): void {
    const location = this.getUniformLocation(name);
    if (location) {
      this.gl.uniform4f(location, x, y, z, w);
    }
  }

  setUniformMatrix4fv(name: string, matrix: Float32Array): void {
    const location = this.getUniformLocation(name);
    if (location) {
      this.gl.uniformMatrix4fv(location, false, matrix);
    }
  }

  dispose(): void {
    if (this.isDisposed) return;
    
    this.gl.deleteProgram(this.program);
    this.uniformLocations.clear();
    this.attributeLocations.clear();
    this.isDisposed = true;
  }
}
