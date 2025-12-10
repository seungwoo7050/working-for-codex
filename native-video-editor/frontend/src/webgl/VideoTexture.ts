// [FILE]
// - 목적: HTML5 Video 요소를 WebGL 텍스처로 변환
// - 주요 역할: 비디오 프레임을 GPU에 업로드하여 렌더링에 사용
// - 관련 클론 가이드 단계: [CG-v3.1.2] WebGL 텍스처 관리
// - 권장 읽는 순서: Order 1 (생성) → Order 2 (업데이트) → Order 3 (바인딩)
//
// [LEARN] C 개발자를 위한 비디오 텍스처 이해:
// - 비디오 프레임을 GPU 메모리에 업로드하여 셰이더에서 접근할 수 있게 한다.
// - C/OpenGL에서 glTexImage2D()로 이미지 데이터를 텍스처에 업로드하는 것과 동일하다.
// - WebGL은 HTMLVideoElement를 직접 텍스처 소스로 사용할 수 있다.
// - 매 프레임마다 업데이트해야 비디오가 재생되는 것처럼 보인다.

/**
 * Video Texture - HTML5 Video → WebGL 텍스처 변환
 * @version 3.1.2
 */

export class VideoTexture {
  private gl: WebGL2RenderingContext;
  private texture: WebGLTexture;
  private video: HTMLVideoElement;
  private needsUpdate = true;  // 업데이트 필요 플래그
  private isDisposed = false;

  // [Order 1] 생성자 - 텍스처 초기화
  constructor(gl: WebGL2RenderingContext, video: HTMLVideoElement) {
    this.gl = gl;
    this.video = video;
    this.texture = this.createTexture();
    
    // 비디오 상태 변경 시 업데이트 플래그 설정
    this.video.addEventListener('playing', this.markForUpdate);
    this.video.addEventListener('seeked', this.markForUpdate);
  }

  private markForUpdate = (): void => {
    this.needsUpdate = true;
  };

  // [Order 1-1] 텍스처 객체 생성
  // [LEARN] 텍스처 파라미터
  // - TEXTURE_WRAP_S/T: 텍스처 좌표가 범위를 벗어날 때 처리 방법
  //   - CLAMP_TO_EDGE: 가장자리 색상으로 채움 (비디오에 적합)
  // - TEXTURE_MIN/MAG_FILTER: 축소/확대 시 필터링 방법
  //   - LINEAR: 선형 보간 (부드러운 결과)
  //   - NEAREST: 최근접 픽셀 (픽셀화된 결과)
  private createTexture(): WebGLTexture {
    const texture = this.gl.createTexture();
    if (!texture) {
      throw new Error('Failed to create video texture');
    }

    this.gl.bindTexture(this.gl.TEXTURE_2D, texture);
    
    // Set texture parameters
    this.gl.texParameteri(this.gl.TEXTURE_2D, this.gl.TEXTURE_WRAP_S, this.gl.CLAMP_TO_EDGE);
    this.gl.texParameteri(this.gl.TEXTURE_2D, this.gl.TEXTURE_WRAP_T, this.gl.CLAMP_TO_EDGE);
    this.gl.texParameteri(this.gl.TEXTURE_2D, this.gl.TEXTURE_MIN_FILTER, this.gl.LINEAR);
    this.gl.texParameteri(this.gl.TEXTURE_2D, this.gl.TEXTURE_MAG_FILTER, this.gl.LINEAR);

    // Initialize with empty pixel
    this.gl.texImage2D(
      this.gl.TEXTURE_2D,
      0,
      this.gl.RGBA,
      1,
      1,
      0,
      this.gl.RGBA,
      this.gl.UNSIGNED_BYTE,
      new Uint8Array([0, 0, 0, 255])
    );

    return texture;
  }

  // [Order 2] 텍스처 업데이트 - 비디오 프레임을 GPU에 업로드
  // [LEARN] 비디오 프레임 업로드
  // - texImage2D()의 마지막 인자로 HTMLVideoElement를 직접 전달할 수 있다.
  // - 브라우저가 현재 비디오 프레임을 자동으로 추출하여 업로드한다.
  // - C/OpenGL에서는 직접 프레임 버퍼를 읽어서 업로드해야 한다.
  update(): boolean {
    if (this.isDisposed) return false;
    
    // Check if video has data
    if (this.video.readyState < HTMLMediaElement.HAVE_CURRENT_DATA) {
      return false;
    }

    // Only update if needed or video is playing
    if (!this.needsUpdate && this.video.paused) {
      return false;
    }

    this.gl.bindTexture(this.gl.TEXTURE_2D, this.texture);
    this.gl.texImage2D(
      this.gl.TEXTURE_2D,
      0,
      this.gl.RGBA,
      this.gl.RGBA,
      this.gl.UNSIGNED_BYTE,
      this.video
    );

    this.needsUpdate = false;
    return true;
  }

  // [Order 3] 텍스처 바인딩
  // [LEARN] 텍스처 유닛
  // - GPU는 여러 텍스처를 동시에 바인딩할 수 있다 (TEXTURE0, TEXTURE1, ...)
  // - 셰이더에서 sampler2D 유니폼에 텍스처 유닛 번호를 전달한다.
  // - C/OpenGL의 glActiveTexture(GL_TEXTURE0 + unit)과 동일하다.
  bind(unit: number = 0): void {
    this.gl.activeTexture(this.gl.TEXTURE0 + unit);
    this.gl.bindTexture(this.gl.TEXTURE_2D, this.texture);
  }

  getTexture(): WebGLTexture {
    return this.texture;
  }

  getVideo(): HTMLVideoElement {
    return this.video;
  }

  getWidth(): number {
    return this.video.videoWidth;
  }

  getHeight(): number {
    return this.video.videoHeight;
  }

  dispose(): void {
    if (this.isDisposed) return;
    
    this.video.removeEventListener('playing', this.markForUpdate);
    this.video.removeEventListener('seeked', this.markForUpdate);
    this.gl.deleteTexture(this.texture);
    this.isDisposed = true;
  }
}
