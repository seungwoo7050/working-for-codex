// [FILE]
// - 목적: WebAudio 모듈의 진입점 및 전체 API 내보내기 (Barrel Export)
// - 주요 역할: 오디오 처리 관련 클래스들을 하나의 모듈로 통합하여 외부에 노출
// - 관련 클론 가이드 단계: [CG-v3.2.0] ~ [CG-v3.2.4] WebAudio 엔진 시리즈
// - 권장 읽는 순서: 이 파일 → AudioContextManager.ts → AudioNodeGraph.ts → FFTAnalyzer.ts
//
// [LEARN] C 개발자를 위한 Web Audio API 이해:
// - Web Audio API는 브라우저에서 저수준 오디오 처리를 가능하게 하는 API이다.
// - C에서 PortAudio, OpenAL, ALSA 등을 사용하는 것과 유사한 개념이다.
// - 노드 기반 오디오 그래프: 오디오 소스 → 이펙트 → 출력 (destination)
// - DSP(디지털 신호 처리) 개념: FFT, 필터, 게인, 컴프레서 등
//
// [LEARN] 오디오 그래프 개념
// - 오디오 노드들을 연결하여 신호 처리 체인을 구성한다.
// - C로 비유하면, 함수 포인터 체인으로 오디오 버퍼를 순차 처리하는 것과 유사하다.
// - 예: AudioBufferSourceNode → GainNode → AnalyserNode → AudioDestinationNode

/**
 * WebAudio Module Exports
 * @version 3.2.4
 */

// Core Engine (v3.2.0) - AudioContext 관리, 버퍼 로딩, 디버깅
export { AudioContextManager, type AudioContextOptions } from './AudioContextManager';
export { AudioLoader, type LoadProgress } from './AudioLoader';
export { AudioBufferManager, type BufferInfo } from './AudioBufferManager';
export { AudioDebug, type AudioDebugInfo } from './AudioDebug';

// Audio Node System (v3.2.1) - 노드 그래프, 커스텀 노드, 연결 관리
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

// Realtime Audio Processing (v3.2.2) - 실시간 처리, FFT 분석, 필터
export { ScriptProcessorWrapper, type ProcessCallback } from './ScriptProcessorWrapper';
export { AudioWorkletManager, type WorkletProcessorInfo } from './AudioWorkletManager';
export { FFTAnalyzer, type FFTData, type FrequencyBands } from './FFTAnalyzer';
export {
  AudioFilters,
  type FilterPreset,
  type FilterConfig,
  type EQBand,
} from './AudioFilters';

// Audio Visualization (v3.2.3) - 파형, 스펙트로그램, 실시간 시각화
export { WaveformRenderer, type WaveformStyle } from './WaveformRenderer';
export { SpectrogramRenderer, type SpectrogramStyle } from './SpectrogramRenderer';
export {
  RealtimeVisualizer,
  type VisualizationType,
  type VisualizerStyle,
} from './RealtimeVisualizer';
export { WebGLAudioVisualizer, type WebGLVisualizerOptions } from './WebGLAudioVisualizer';

// Performance Optimization (v3.2.4) - 버퍼 풀링, 노드 풀링, 메모리 관리
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

// [Reader Notes]
// =============================================================================
// 이 파일에서 처음 등장한 개념: Web Audio API, 오디오 그래프, DSP
//
// ## Web Audio API 아키텍처 (C 개발자 관점)
// - AudioContext: 오디오 시스템의 진입점 (ALSA의 snd_pcm_open과 유사)
// - AudioNode: 오디오 처리 단위 (필터, 게인, 분석기 등)
// - AudioBuffer: 메모리에 로드된 오디오 데이터 (float32 PCM)
// - AudioDestinationNode: 최종 출력 (스피커)
//
// ## 이 모듈의 계층 구조
// 1. Core Engine: AudioContext 관리, 버퍼 로딩
// 2. Node System: 노드 그래프 구성, 커스텀 노드
// 3. Realtime Processing: FFT, 필터, AudioWorklet
// 4. Visualization: 파형, 스펙트로그램 렌더링
// 5. Performance: 오브젝트 풀링, 메모리 관리
//
// ## 이 파일을 이해한 다음, 이어서 보면 좋은 파일:
// 1. AudioContextManager.ts - AudioContext 생성 및 상태 관리
// 2. AudioNodeGraph.ts - 노드 연결 그래프 관리
// 3. FFTAnalyzer.ts - 주파수 분석
// =============================================================================
