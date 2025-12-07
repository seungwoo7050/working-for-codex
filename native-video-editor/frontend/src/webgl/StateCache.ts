/**
 * State Cache - WebGL 상태 캐싱
 * @version 3.1.4
 */

export interface BlendState {
  enabled: boolean;
  srcRGB: number;
  dstRGB: number;
  srcAlpha: number;
  dstAlpha: number;
}

export interface DepthState {
  testEnabled: boolean;
  writeEnabled: boolean;
  func: number;
}

export interface CullState {
  enabled: boolean;
  face: number;
}

export class StateCache {
  private gl: WebGL2RenderingContext;
  
  // Cached states
  private currentProgram: WebGLProgram | null = null;
  private currentVAO: WebGLVertexArrayObject | null = null;
  private currentFramebuffer: WebGLFramebuffer | null = null;
  private boundTextures: Map<number, WebGLTexture | null> = new Map();
  private activeTextureUnit: number = 0;
  
  private blendState: BlendState = {
    enabled: false,
    srcRGB: 0,
    dstRGB: 0,
    srcAlpha: 0,
    dstAlpha: 0,
  };
  
  private depthState: DepthState = {
    testEnabled: false,
    writeEnabled: true,
    func: 0,
  };
  
  private cullState: CullState = {
    enabled: false,
    face: 0,
  };

  private viewport: { x: number; y: number; width: number; height: number } = {
    x: 0, y: 0, width: 0, height: 0
  };

  private stateChanges = 0;
  private stateSaved = 0;

  constructor(gl: WebGL2RenderingContext) {
    this.gl = gl;
    this.initializeState();
  }

  private initializeState(): void {
    const gl = this.gl;
    
    this.blendState = {
      enabled: gl.isEnabled(gl.BLEND),
      srcRGB: gl.getParameter(gl.BLEND_SRC_RGB),
      dstRGB: gl.getParameter(gl.BLEND_DST_RGB),
      srcAlpha: gl.getParameter(gl.BLEND_SRC_ALPHA),
      dstAlpha: gl.getParameter(gl.BLEND_DST_ALPHA),
    };
    
    this.depthState = {
      testEnabled: gl.isEnabled(gl.DEPTH_TEST),
      writeEnabled: gl.getParameter(gl.DEPTH_WRITEMASK),
      func: gl.getParameter(gl.DEPTH_FUNC),
    };
    
    this.cullState = {
      enabled: gl.isEnabled(gl.CULL_FACE),
      face: gl.getParameter(gl.CULL_FACE_MODE),
    };

    const vp = gl.getParameter(gl.VIEWPORT);
    this.viewport = { x: vp[0], y: vp[1], width: vp[2], height: vp[3] };
  }

  useProgram(program: WebGLProgram | null): void {
    if (this.currentProgram !== program) {
      this.gl.useProgram(program);
      this.currentProgram = program;
      this.stateChanges++;
    } else {
      this.stateSaved++;
    }
  }

  bindVAO(vao: WebGLVertexArrayObject | null): void {
    if (this.currentVAO !== vao) {
      this.gl.bindVertexArray(vao);
      this.currentVAO = vao;
      this.stateChanges++;
    } else {
      this.stateSaved++;
    }
  }

  bindFramebuffer(target: number, framebuffer: WebGLFramebuffer | null): void {
    if (this.currentFramebuffer !== framebuffer) {
      this.gl.bindFramebuffer(target, framebuffer);
      this.currentFramebuffer = framebuffer;
      this.stateChanges++;
    } else {
      this.stateSaved++;
    }
  }

  activeTexture(unit: number): void {
    if (this.activeTextureUnit !== unit) {
      this.gl.activeTexture(this.gl.TEXTURE0 + unit);
      this.activeTextureUnit = unit;
      this.stateChanges++;
    } else {
      this.stateSaved++;
    }
  }

  bindTexture(target: number, texture: WebGLTexture | null): void {
    const key = this.activeTextureUnit * 1000 + target;
    if (this.boundTextures.get(key) !== texture) {
      this.gl.bindTexture(target, texture);
      this.boundTextures.set(key, texture);
      this.stateChanges++;
    } else {
      this.stateSaved++;
    }
  }

  setBlend(enabled: boolean, srcRGB?: number, dstRGB?: number, srcAlpha?: number, dstAlpha?: number): void {
    if (this.blendState.enabled !== enabled) {
      if (enabled) {
        this.gl.enable(this.gl.BLEND);
      } else {
        this.gl.disable(this.gl.BLEND);
      }
      this.blendState.enabled = enabled;
      this.stateChanges++;
    } else {
      this.stateSaved++;
    }

    if (enabled && srcRGB !== undefined && dstRGB !== undefined) {
      const src = srcRGB;
      const dst = dstRGB;
      const srcA = srcAlpha ?? srcRGB;
      const dstA = dstAlpha ?? dstRGB;
      
      if (this.blendState.srcRGB !== src || 
          this.blendState.dstRGB !== dst ||
          this.blendState.srcAlpha !== srcA ||
          this.blendState.dstAlpha !== dstA) {
        this.gl.blendFuncSeparate(src, dst, srcA, dstA);
        this.blendState.srcRGB = src;
        this.blendState.dstRGB = dst;
        this.blendState.srcAlpha = srcA;
        this.blendState.dstAlpha = dstA;
        this.stateChanges++;
      }
    }
  }

  setDepthTest(enabled: boolean, func?: number): void {
    if (this.depthState.testEnabled !== enabled) {
      if (enabled) {
        this.gl.enable(this.gl.DEPTH_TEST);
      } else {
        this.gl.disable(this.gl.DEPTH_TEST);
      }
      this.depthState.testEnabled = enabled;
      this.stateChanges++;
    }

    if (enabled && func !== undefined && this.depthState.func !== func) {
      this.gl.depthFunc(func);
      this.depthState.func = func;
      this.stateChanges++;
    }
  }

  setDepthMask(enabled: boolean): void {
    if (this.depthState.writeEnabled !== enabled) {
      this.gl.depthMask(enabled);
      this.depthState.writeEnabled = enabled;
      this.stateChanges++;
    }
  }

  setCullFace(enabled: boolean, face?: number): void {
    if (this.cullState.enabled !== enabled) {
      if (enabled) {
        this.gl.enable(this.gl.CULL_FACE);
      } else {
        this.gl.disable(this.gl.CULL_FACE);
      }
      this.cullState.enabled = enabled;
      this.stateChanges++;
    }

    if (enabled && face !== undefined && this.cullState.face !== face) {
      this.gl.cullFace(face);
      this.cullState.face = face;
      this.stateChanges++;
    }
  }

  setViewport(x: number, y: number, width: number, height: number): void {
    if (this.viewport.x !== x || 
        this.viewport.y !== y || 
        this.viewport.width !== width || 
        this.viewport.height !== height) {
      this.gl.viewport(x, y, width, height);
      this.viewport = { x, y, width, height };
      this.stateChanges++;
    } else {
      this.stateSaved++;
    }
  }

  invalidate(): void {
    this.currentProgram = null;
    this.currentVAO = null;
    this.currentFramebuffer = null;
    this.boundTextures.clear();
    this.initializeState();
  }

  getStats(): { changes: number; saved: number; efficiency: number } {
    const total = this.stateChanges + this.stateSaved;
    return {
      changes: this.stateChanges,
      saved: this.stateSaved,
      efficiency: total > 0 ? this.stateSaved / total : 0,
    };
  }

  resetStats(): void {
    this.stateChanges = 0;
    this.stateSaved = 0;
  }
}
