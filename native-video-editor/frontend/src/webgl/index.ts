/**
 * WebGL Module Exports
 * @version 3.1.4
 */

// Core Engine (v3.1.0)
export { WebGLEngine, type WebGLEngineOptions } from './WebGLEngine';
export { WebGLExtensions, type ExtensionSupport } from './WebGLExtensions';
export { WebGLDebug, type WebGLDebugOptions } from './WebGLDebug';
export { WebGLMemoryManager, type ResourceStats } from './WebGLMemoryManager';

// Shader System (v3.1.1)
export { ShaderCompiler } from './ShaderCompiler';
export { ShaderProgram, type ShaderProgramOptions } from './ShaderProgram';
export { UniformBinder } from './UniformBinder';
export { ShaderCache } from './ShaderCache';

// Texture Management (v3.1.2)
export { VideoTexture, type VideoTextureOptions } from './VideoTexture';
export { TextureFilter, FilterMode, WrapMode } from './TextureFilter';
export { MipmapGenerator } from './MipmapGenerator';
export { TextureCompressor, type CompressionSupport } from './TextureCompressor';

// Rendering Pipeline (v3.1.3)
export { Framebuffer, type FramebufferOptions } from './Framebuffer';
export { Renderbuffer, type RenderbufferFormat } from './Renderbuffer';
export { MultiPassRenderer, type RenderPass } from './MultiPassRenderer';
export { PostProcessor, type PostEffect } from './PostProcessor';

// Performance Optimization (v3.1.4)
export { StateCache, type BlendState, type DepthState, type CullState } from './StateCache';
export { BatchRenderer, type BatchItem } from './BatchRenderer';
export { GPUMemoryOptimizer, type TextureInfo, type MemoryStats } from './GPUMemoryOptimizer';
export { WebGLProfiler, type FrameStats, type ProfilerReport } from './WebGLProfiler';
