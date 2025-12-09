// [FILE]
// - 목적: WebGL2 렌더링 컨텍스트 생성 및 관리
// - 주요 역할: Canvas에서 WebGL2 컨텍스트 획득, 기본 설정, 컨텍스트 손실 처리
// - 관련 클론 가이드 단계: [CG-v3.1.0] WebGL 코어 엔진
// - 권장 읽는 순서: Order 1 (생성자) → Order 2 (컨텍스트 관리) → Order 3 (유틸리티)
//
// [LEARN] C 개발자를 위한 WebGL 컨텍스트 이해:
// - WebGL 컨텍스트는 OpenGL의 렌더링 컨텍스트와 동일한 개념이다.
// - C에서 glutCreateWindow() + glutCreateContext()로 OpenGL 컨텍스트를 만드는 것처럼,
//   JavaScript에서는 canvas.getContext('webgl2')로 생성한다.
// - 컨텍스트는 GPU와의 연결을 나타내며, 모든 렌더링 명령의 대상이 된다.

/**
 * WebGL Engine - WebGL2 컨텍스트 관리
 * @version 3.1.0
 */

// [Order 1] 엔진 옵션 타입 정의
// - OpenGL의 컨텍스트 속성과 유사한 설정들이다.
export interface WebGLEngineOptions {
  antialias?: boolean;           // 안티앨리어싱 활성화
  alpha?: boolean;               // 알파 채널 포함 여부
  depth?: boolean;               // 깊이 버퍼 사용
  stencil?: boolean;             // 스텐실 버퍼 사용
  preserveDrawingBuffer?: boolean; // 드로잉 버퍼 보존 (스크린샷용)
  powerPreference?: 'default' | 'high-performance' | 'low-power';
}

// [Order 2] WebGL 엔진 클래스
export class WebGLEngine {
  private gl: WebGL2RenderingContext;
  private canvas: HTMLCanvasElement;
  private isDisposed = false;

  // [Order 2-1] 생성자 - WebGL2 컨텍스트 획득
  constructor(canvas: HTMLCanvasElement, options?: WebGLEngineOptions) {
    this.canvas = canvas;

    // [LEARN] WebGL 컨텍스트 속성
    // - C/OpenGL의 glutInitDisplayMode()로 설정하는 것과 유사하다.
    // - 예: GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE
    const contextAttributes: WebGLContextAttributes = {
      antialias: options?.antialias ?? true,
      alpha: options?.alpha ?? false,
      depth: options?.depth ?? true,
      stencil: options?.stencil ?? false,
      preserveDrawingBuffer: options?.preserveDrawingBuffer ?? false,
      powerPreference: options?.powerPreference ?? 'high-performance',
    };

    // WebGL2 컨텍스트 획득 (WebGL1은 'webgl')
    const gl = canvas.getContext('webgl2', contextAttributes);
    if (!gl) {
      throw new Error('WebGL2 is not supported in this browser');
    }

    this.gl = gl;
    this.setupContextLossHandling();
  }

  // [Order 2-2] 컨텍스트 손실 처리
  // [LEARN] WebGL 컨텍스트 손실
  // - GPU 드라이버 리셋, 메모리 부족 등의 이유로 컨텍스트가 손실될 수 있다.
  // - C에서 OpenGL 에러 핸들링과 유사하지만, 웹에서는 이벤트로 처리한다.
  // - 손실 시 모든 WebGL 리소스(텍스처, 버퍼, 셰이더)가 무효화된다.
  private setupContextLossHandling(): void {
    this.canvas.addEventListener('webglcontextlost', (event) => {
      event.preventDefault();
      console.warn('[WebGLEngine] Context lost');
    });

    this.canvas.addEventListener('webglcontextrestored', () => {
      console.log('[WebGLEngine] Context restored');
      // Re-initialize resources here if needed
    });
  }

  // [Order 3] 공개 API - 컨텍스트 및 유틸리티 메서드
  
  // WebGL 컨텍스트 반환 (다른 모듈에서 gl 명령 호출 시 사용)
  getContext(): WebGL2RenderingContext {
    if (this.isDisposed) {
      throw new Error('WebGLEngine has been disposed');
    }
    return this.gl;
  }

  getCanvas(): HTMLCanvasElement {
    return this.canvas;
  }

  // [LEARN] 뷰포트 설정
  // - C/OpenGL의 glViewport(0, 0, width, height)와 동일하다.
  // - 캔버스 크기가 변경되면 뷰포트도 업데이트해야 한다.
  resize(width: number, height: number): void {
    this.canvas.width = width;
    this.canvas.height = height;
    this.gl.viewport(0, 0, width, height);
  }

  // [LEARN] 화면 지우기
  // - glClearColor() + glClear()와 동일하다.
  // - COLOR_BUFFER_BIT: 색상 버퍼 클리어
  // - DEPTH_BUFFER_BIT: 깊이 버퍼 클리어
  clear(r = 0, g = 0, b = 0, a = 1): void {
    this.gl.clearColor(r, g, b, a);
    this.gl.clear(this.gl.COLOR_BUFFER_BIT | this.gl.DEPTH_BUFFER_BIT);
  }

  isContextLost(): boolean {
    return this.gl.isContextLost();
  }

  // [LEARN] 리소스 정리
  // - C에서 free()나 glDeleteTextures()처럼 리소스를 해제해야 한다.
  // - JavaScript는 가비지 컬렉션이 있지만, WebGL 리소스는 명시적 해제가 권장된다.
  dispose(): void {
    if (this.isDisposed) return;
    
    // WebGL context will be garbage collected with canvas
    // but we mark as disposed to prevent further use
    this.isDisposed = true;
  }
}

// [Reader Notes]
// =============================================================================
// 이 파일에서 처음 등장한 개념: WebGL2 컨텍스트, 컨텍스트 속성, 컨텍스트 손실
//
// ## WebGL vs. OpenGL 비교 (C 개발자 관점)
// | 개념 | C/OpenGL | JavaScript/WebGL |
// |------|----------|------------------|
// | 컨텍스트 생성 | glutCreateContext() | canvas.getContext('webgl2') |
// | 뷰포트 설정 | glViewport() | gl.viewport() |
// | 화면 클리어 | glClear() | gl.clear() |
// | 에러 확인 | glGetError() | gl.getError() |
//
// ## 컨텍스트 손실 대응 전략
// 1. webglcontextlost 이벤트 수신
// 2. event.preventDefault()로 자동 복원 시도
// 3. webglcontextrestored 이벤트에서 리소스 재생성
//
// ## 이 파일을 이해한 다음, 이어서 보면 좋은 파일:
// 1. ShaderCompiler.ts - GLSL 셰이더 컴파일
// 2. VideoTexture.ts - 비디오를 텍스처로 변환
// =============================================================================
