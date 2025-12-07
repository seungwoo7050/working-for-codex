/**
 * Video Texture - HTML5 Video → WebGL 텍스처 변환
 * @version 3.1.2
 */

export class VideoTexture {
  private gl: WebGL2RenderingContext;
  private texture: WebGLTexture;
  private video: HTMLVideoElement;
  private needsUpdate = true;
  private isDisposed = false;

  constructor(gl: WebGL2RenderingContext, video: HTMLVideoElement) {
    this.gl = gl;
    this.video = video;
    this.texture = this.createTexture();
    
    // Update on video frame change
    this.video.addEventListener('playing', this.markForUpdate);
    this.video.addEventListener('seeked', this.markForUpdate);
  }

  private markForUpdate = (): void => {
    this.needsUpdate = true;
  };

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
