// [FILE]
// - 목적: 오프스크린 렌더링을 위한 Framebuffer 래퍼
// - 주요 역할: FBO 생성, 컬러/깊이 어태치먼트 관리, 렌더 타겟 전환
// - 관련 클론 가이드 단계: [CG-v3.1.3] WebGL 렌더링 파이프라인
// - 권장 읽는 순서: WebGLEngine.ts 이후
//
// [LEARN] C 개발자를 위한 Framebuffer Object (FBO):
// - 기본적으로 WebGL은 화면(Default Framebuffer)에 렌더링한다.
// - FBO를 사용하면 텍스처에 렌더링할 수 있다 (Off-screen Rendering).
// - 후처리 효과(블러, 글로우 등)를 구현할 때 필수적이다.
// - C에서 OpenGL FBO를 사용하는 것과 동일한 개념이다.
//
// [Reader Notes]
// - FBO는 "렌더 타겟"을 텍스처로 설정하는 기법이다.
// - 컬러 버퍼: 픽셀 색상을 저장 (RGBA 텍스처)
// - 깊이 버퍼: 깊이 값을 저장 (Renderbuffer 또는 텍스처)
// - 여러 패스 렌더링에서 중간 결과를 저장하는 데 사용된다.

/**
 * Framebuffer - 오프스크린 렌더링
 * @version 3.1.3
 */

// [LEARN] 인터페이스로 옵션 구조 정의
// - C에서 함수에 옵션을 전달할 때 구조체를 사용하는 것과 유사하다.
export interface FramebufferOptions {
  width: number;             // FBO 너비 (픽셀)
  height: number;            // FBO 높이 (픽셀)
  colorFormat?: number;      // 컬러 포맷 (기본: RGBA8)
  depthBuffer?: boolean;     // 깊이 버퍼 사용 여부
  stencilBuffer?: boolean;   // 스텐실 버퍼 사용 여부
}

export class Framebuffer {
  private gl: WebGL2RenderingContext;
  private fbo: WebGLFramebuffer;          // Framebuffer Object 핸들
  private colorTexture: WebGLTexture;     // 컬러 어태치먼트 (텍스처)
  private depthRenderbuffer: WebGLRenderbuffer | null = null;  // 깊이 어태치먼트
  private width: number;
  private height: number;
  private isDisposed = false;

  constructor(gl: WebGL2RenderingContext, options: FramebufferOptions) {
    this.gl = gl;
    this.width = options.width;
    this.height = options.height;

    // [LEARN] Framebuffer 생성
    // - gl.createFramebuffer()로 FBO 객체를 생성한다.
    // - C의 glGenFramebuffers()와 동일하다.
    // Create framebuffer
    const fbo = gl.createFramebuffer();
    if (!fbo) {
      throw new Error('Failed to create framebuffer');
    }
    this.fbo = fbo;

    // [LEARN] 컬러 텍스처 생성 (렌더 타겟)
    // - FBO에 연결할 텍스처를 생성한다.
    // - 이 텍스처에 렌더링 결과가 저장된다.
    // Create color attachment
    const colorTexture = gl.createTexture();
    if (!colorTexture) {
      gl.deleteFramebuffer(fbo);
      throw new Error('Failed to create color texture');
    }
    this.colorTexture = colorTexture;

    // [LEARN] 텍스처 설정
    // - texImage2D의 마지막 인자가 null이면 빈 텍스처를 할당한다.
    // - 렌더링 시 GPU가 이 메모리에 픽셀을 기록한다.
    // Setup color texture
    gl.bindTexture(gl.TEXTURE_2D, colorTexture);
    gl.texImage2D(
      gl.TEXTURE_2D,
      0,
      options.colorFormat ?? gl.RGBA8,  // 내부 포맷
      options.width,
      options.height,
      0,
      gl.RGBA,           // 데이터 포맷
      gl.UNSIGNED_BYTE,  // 데이터 타입
      null               // 초기 데이터 없음
    );
    // [LEARN] 텍스처 필터링 설정
    // - CLAMP_TO_EDGE: 텍스처 좌표가 0~1 범위를 벗어나면 가장자리 색상 사용
    gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.LINEAR);
    gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MAG_FILTER, gl.LINEAR);
    gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_S, gl.CLAMP_TO_EDGE);
    gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_T, gl.CLAMP_TO_EDGE);

    // [LEARN] FBO에 텍스처 연결 (Attach)
    // - COLOR_ATTACHMENT0: 0번 컬러 출력에 텍스처를 연결한다.
    // - 프래그먼트 셰이더의 출력이 이 텍스처에 기록된다.
    // Attach to framebuffer
    gl.bindFramebuffer(gl.FRAMEBUFFER, fbo);
    gl.framebufferTexture2D(
      gl.FRAMEBUFFER,
      gl.COLOR_ATTACHMENT0,  // 어태치먼트 포인트
      gl.TEXTURE_2D,
      colorTexture,
      0  // Mipmap 레벨
    );

    // [LEARN] 깊이/스텐실 버퍼 (선택적)
    // - 3D 렌더링에서 깊이 테스트가 필요하면 깊이 버퍼를 추가한다.
    // - Renderbuffer는 텍스처처럼 샘플링할 수 없지만 더 효율적이다.
    // Create depth/stencil buffer if requested
    if (options.depthBuffer || options.stencilBuffer) {
      const rbo = gl.createRenderbuffer();
      if (!rbo) {
        throw new Error('Failed to create renderbuffer');
      }
      this.depthRenderbuffer = rbo;

      gl.bindRenderbuffer(gl.RENDERBUFFER, rbo);
      
      const format = options.stencilBuffer 
        ? gl.DEPTH24_STENCIL8 
        : gl.DEPTH_COMPONENT24;
      
      gl.renderbufferStorage(gl.RENDERBUFFER, format, options.width, options.height);

      const attachment = options.stencilBuffer 
        ? gl.DEPTH_STENCIL_ATTACHMENT 
        : gl.DEPTH_ATTACHMENT;
      
      gl.framebufferRenderbuffer(gl.FRAMEBUFFER, attachment, gl.RENDERBUFFER, rbo);
    }

    // Check completeness
    const status = gl.checkFramebufferStatus(gl.FRAMEBUFFER);
    if (status !== gl.FRAMEBUFFER_COMPLETE) {
      this.dispose();
      throw new Error(`Framebuffer not complete: ${status}`);
    }

    // Unbind
    gl.bindFramebuffer(gl.FRAMEBUFFER, null);
  }

  bind(): void {
    this.gl.bindFramebuffer(this.gl.FRAMEBUFFER, this.fbo);
    this.gl.viewport(0, 0, this.width, this.height);
  }

  unbind(): void {
    this.gl.bindFramebuffer(this.gl.FRAMEBUFFER, null);
  }

  getColorTexture(): WebGLTexture {
    return this.colorTexture;
  }

  getFramebuffer(): WebGLFramebuffer {
    return this.fbo;
  }

  getWidth(): number {
    return this.width;
  }

  getHeight(): number {
    return this.height;
  }

  resize(width: number, height: number): void {
    this.width = width;
    this.height = height;

    // Resize color texture
    this.gl.bindTexture(this.gl.TEXTURE_2D, this.colorTexture);
    this.gl.texImage2D(
      this.gl.TEXTURE_2D,
      0,
      this.gl.RGBA8,
      width,
      height,
      0,
      this.gl.RGBA,
      this.gl.UNSIGNED_BYTE,
      null
    );

    // Resize depth buffer if exists
    if (this.depthRenderbuffer) {
      this.gl.bindRenderbuffer(this.gl.RENDERBUFFER, this.depthRenderbuffer);
      this.gl.renderbufferStorage(
        this.gl.RENDERBUFFER,
        this.gl.DEPTH_COMPONENT24,
        width,
        height
      );
    }
  }

  dispose(): void {
    if (this.isDisposed) return;

    this.gl.deleteTexture(this.colorTexture);
    if (this.depthRenderbuffer) {
      this.gl.deleteRenderbuffer(this.depthRenderbuffer);
    }
    this.gl.deleteFramebuffer(this.fbo);
    
    this.isDisposed = true;
  }
}
