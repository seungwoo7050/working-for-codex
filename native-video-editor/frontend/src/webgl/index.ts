// [FILE]
// - 목적: WebGL 모듈의 진입점 및 전체 API 내보내기 (Barrel Export)
// - 주요 역할: WebGL 관련 클래스들을 하나의 모듈로 통합하여 외부에 노출
// - 관련 클론 가이드 단계: [CG-v3.1.0] ~ [CG-v3.1.4] WebGL 엔진 시리즈
// - 권장 읽는 순서: 이 파일 → WebGLEngine.ts → ShaderProgram.ts → VideoTexture.ts
//
// [LEARN] C 개발자를 위한 WebGL 이해:
// - WebGL은 OpenGL ES 2.0/3.0을 JavaScript에서 사용할 수 있게 한 API이다.
// - C/C++에서 OpenGL을 사용하는 것과 거의 동일한 개념과 함수 이름을 사용한다.
// - GPU를 직접 프로그래밍하여 고성능 그래픽스 처리가 가능하다.
// - 이 프로젝트에서는 비디오 렌더링, 이펙트, 필터 등에 활용한다.
//
// [LEARN] Barrel Export 패턴
// - 모든 export를 하나의 index.ts 파일에 모아서 re-export한다.
// - import { WebGLEngine, ShaderProgram } from './webgl' 처럼 간결하게 사용 가능하다.
// - C의 헤더 파일에서 여러 모듈의 선언을 모으는 것과 유사하다.

/**
 * WebGL Module Exports
 * @version 3.1.4
 */

// Core Engine (v3.1.0) - WebGL 컨텍스트 관리, 초기화, 디버깅
export { WebGLEngine, type WebGLEngineOptions } from './WebGLEngine';
export { WebGLExtensions, type ExtensionSupport } from './WebGLExtensions';
export { WebGLDebug, type WebGLDebugOptions } from './WebGLDebug';
export { WebGLMemoryManager, type ResourceStats } from './WebGLMemoryManager';

// Shader System (v3.1.1) - GLSL 셰이더 컴파일, 링킹, 유니폼 관리
export { ShaderCompiler } from './ShaderCompiler';
export { ShaderProgram, type ShaderProgramOptions } from './ShaderProgram';
export { UniformBinder } from './UniformBinder';
export { ShaderCache } from './ShaderCache';

// Texture Management (v3.1.2) - 비디오 텍스처, 필터링, 압축
export { VideoTexture } from './VideoTexture';
export { TextureFilter, type FilterMode, type WrapMode } from './TextureFilter';
export { MipmapGenerator } from './MipmapGenerator';
export { TextureCompressor, type CompressionInfo } from './TextureCompressor';

// Rendering Pipeline (v3.1.3) - 프레임버퍼, 멀티패스 렌더링, 후처리
export { Framebuffer, type FramebufferOptions } from './Framebuffer';
export { Renderbuffer, type RenderbufferFormat } from './Renderbuffer';
export { MultiPassRenderer, type RenderPass } from './MultiPassRenderer';
export { PostProcessor, type PostEffect } from './PostProcessor';

// Performance Optimization (v3.1.4) - 상태 캐싱, 배칭, 메모리 최적화
export { StateCache, type BlendState, type DepthState, type CullState } from './StateCache';
export { BatchRenderer, type BatchItem } from './BatchRenderer';
export { GPUMemoryOptimizer, type TextureInfo, type MemoryStats } from './GPUMemoryOptimizer';
export { WebGLProfiler, type FrameStats, type ProfilerReport } from './WebGLProfiler';

// [Reader Notes]
// =============================================================================
// 이 파일에서 처음 등장한 개념: WebGL, GPU 프로그래밍, 셰이더
//
// ## WebGL 아키텍처 (C/OpenGL 개발자 관점)
// - WebGL2 = OpenGL ES 3.0 + JavaScript 바인딩
// - gl.bindBuffer(), gl.bindTexture() 등 OpenGL 함수와 1:1 대응
// - 셰이더는 GLSL (OpenGL Shading Language)로 작성
//
// ## 이 모듈의 계층 구조
// 1. Core Engine: WebGL 컨텍스트 생성 및 관리
// 2. Shader System: GPU 프로그램 컴파일 및 실행
// 3. Texture Management: 이미지/비디오 데이터를 GPU에 업로드
// 4. Rendering Pipeline: 프레임버퍼, 멀티패스 렌더링
// 5. Performance: 상태 캐싱, 배칭, 메모리 관리
//
// ## 이 파일을 이해한 다음, 이어서 보면 좋은 파일:
// 1. WebGLEngine.ts - WebGL 컨텍스트 초기화
// 2. ShaderProgram.ts - 셰이더 프로그램 관리
// 3. VideoTexture.ts - 비디오를 텍스처로 변환
// =============================================================================
