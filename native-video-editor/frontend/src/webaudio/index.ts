/**
 * WebAudio Module Exports
 * @version 3.2.4
 */

// Core Engine (v3.2.0)
export { AudioContextManager, type AudioContextOptions } from './AudioContextManager';
export { AudioLoader, type LoadProgress } from './AudioLoader';
export { AudioBufferManager, type BufferInfo } from './AudioBufferManager';
export { AudioDebug, type AudioDebugInfo } from './AudioDebug';

// Audio Node System (v3.2.1)
export { AudioNodeGraph, type AudioNodeInfo } from './AudioNodeGraph';
export {
  CustomAudioNodes,
  type GainNodeOptions,
  type DelayNodeOptions,
  type FilterNodeOptions,
  type CompressorNodeOptions,
} from './CustomAudioNodes';
export { AudioConnector, type ConnectionTarget, type ConnectionOptions } from './AudioConnector';
export { AudioParameters, type AutomationCurve, type AutomationPoint } from './AudioParameters';

// Realtime Audio Processing (v3.2.2)
export { ScriptProcessorWrapper, type ProcessCallback } from './ScriptProcessorWrapper';
export { AudioWorkletManager, type WorkletProcessorInfo } from './AudioWorkletManager';
export { FFTAnalyzer, type FFTData, type FrequencyBands } from './FFTAnalyzer';
export {
  AudioFilters,
  type FilterPreset,
  type FilterConfig,
  type EQBand,
} from './AudioFilters';

// Audio Visualization (v3.2.3)
export { WaveformRenderer, type WaveformStyle } from './WaveformRenderer';
export { SpectrogramRenderer, type SpectrogramStyle } from './SpectrogramRenderer';
export {
  RealtimeVisualizer,
  type VisualizationType,
  type VisualizerStyle,
} from './RealtimeVisualizer';
export { WebGLAudioVisualizer, type WebGLVisualizerOptions } from './WebGLAudioVisualizer';

// Performance Optimization (v3.2.4)
export { AudioBufferPool, type PooledBuffer } from './AudioBufferPool';
export {
  AudioNodePool,
  AudioNodePoolManager,
  type NodeFactory,
  type NodeResetter,
  type PooledNode,
} from './AudioNodePool';
export { AudioMemoryManager, type AudioMemoryStats } from './AudioMemoryManager';
export {
  AudioPerformanceProfiler,
  type AudioPerformanceMetrics,
  type CallbackTiming,
} from './AudioPerformanceProfiler';
