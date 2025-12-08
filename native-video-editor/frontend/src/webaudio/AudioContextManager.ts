// [FILE]
// - 목적: AudioContext 생성 및 생명주기 관리
// - 주요 역할: AudioContext 싱글톤 관리, 자동 resume, 상태 변경 알림
// - 관련 클론 가이드 단계: [CG-v3.2.0] WebAudio 코어 엔진
// - 권장 읽는 순서: Order 1 (옵션) → Order 2 (컨텍스트 관리) → Order 3 (상태 관리)
//
// [LEARN] C 개발자를 위한 AudioContext 이해:
// - AudioContext는 Web Audio API의 진입점이자 오디오 처리의 중심이다.
// - C에서 오디오 디바이스를 열고 설정하는 과정과 유사하다.
//   예: snd_pcm_open(), snd_pcm_hw_params_set_rate() (ALSA)
// - 브라우저 보안 정책: 사용자 상호작용 없이는 오디오 재생 불가 (auto-resume 필요)

/**
 * Audio Context Manager - AudioContext 관리
 * @version 3.2.0
 */

// [Order 1] AudioContext 옵션 타입
export interface AudioContextOptions {
  sampleRate?: number;  // 샘플레이트 (기본: 44100Hz 또는 48000Hz)
  latencyHint?: 'interactive' | 'balanced' | 'playback';  // 지연 시간 힌트
}

// [Order 2] AudioContext 관리 클래스
export class AudioContextManager {
  private context: AudioContext | null = null;
  private isResumed: boolean = false;
  // [LEARN] Set 자료구조
  // - 중복 없는 값들의 집합. C의 해시 테이블과 유사하다.
  // - 리스너 패턴: 상태 변경 시 등록된 콜백들을 호출한다.
  private stateChangeListeners: Set<(state: AudioContextState) => void> = new Set();

  constructor(private options: AudioContextOptions = {}) {}

  // [Order 2-1] AudioContext 획득 (지연 생성)
  // [LEARN] 지연 초기화(Lazy Initialization) 패턴
  // - 필요할 때까지 리소스 생성을 미룬다.
  // - 브라우저 정책: 사용자 상호작용 전에는 AudioContext가 suspended 상태이다.
  async getContext(): Promise<AudioContext> {
    if (!this.context) {
      // [LEARN] AudioContext 생성자 옵션
      // - sampleRate: 오디오 샘플링 레이트 (Hz)
      // - latencyHint: 지연 시간과 CPU 사용량 트레이드오프
      //   - 'interactive': 낮은 지연 (게임, 악기 앱)
      //   - 'playback': 높은 지연 허용 (음악 재생)
      this.context = new AudioContext({
        sampleRate: this.options.sampleRate,
        latencyHint: this.options.latencyHint ?? 'interactive',
      });
      
      // 상태 변경 이벤트 연결
      this.context.onstatechange = () => {
        this.notifyStateChange();
      };
    }

    // [LEARN] AudioContext 자동 resume
    // - 브라우저는 사용자 상호작용 없이 오디오 재생을 차단한다.
    // - 첫 사용자 이벤트(클릭 등) 후 resume()을 호출해야 한다.
    if (this.context.state === 'suspended' && !this.isResumed) {
      await this.resume();
    }

    return this.context;
  }

  // [Order 3] 상태 관리 메서드들
  async resume(): Promise<void> {
    if (this.context && this.context.state === 'suspended') {
      await this.context.resume();
      this.isResumed = true;
    }
  }

  async suspend(): Promise<void> {
    if (this.context && this.context.state === 'running') {
      await this.context.suspend();
    }
  }

  async close(): Promise<void> {
    if (this.context) {
      await this.context.close();
      this.context = null;
      this.isResumed = false;
    }
  }

  getState(): AudioContextState | null {
    return this.context?.state ?? null;
  }

  getSampleRate(): number {
    return this.context?.sampleRate ?? this.options.sampleRate ?? 44100;
  }

  getCurrentTime(): number {
    return this.context?.currentTime ?? 0;
  }

  getDestination(): AudioDestinationNode | null {
    return this.context?.destination ?? null;
  }

  onStateChange(callback: (state: AudioContextState) => void): () => void {
    this.stateChangeListeners.add(callback);
    return () => this.stateChangeListeners.delete(callback);
  }

  private notifyStateChange(): void {
    const state = this.context?.state;
    if (state) {
      this.stateChangeListeners.forEach(cb => cb(state));
    }
  }

  isAvailable(): boolean {
    return typeof AudioContext !== 'undefined';
  }

  getBaseLatency(): number {
    return this.context?.baseLatency ?? 0;
  }

  getOutputLatency(): number {
    const ctx = this.context as (AudioContext & { outputLatency?: number }) | null;
    return ctx?.outputLatency ?? 0;
  }
}

// [Reader Notes]
// =============================================================================
// 이 파일에서 처음 등장한 개념: AudioContext, 오디오 상태 관리, 브라우저 자동재생 정책
//
// ## AudioContext 상태 (C 개발자 관점)
// - 'suspended': 오디오 처리 일시 중지 (브라우저 정책 또는 suspend() 호출)
// - 'running': 정상 동작 중
// - 'closed': 종료됨 (재사용 불가)
// - C에서 오디오 디바이스의 open/close 상태와 유사하다.
//
// ## 브라우저 자동재생 정책
// - 사용자 상호작용(클릭, 터치) 없이는 오디오 재생 불가
// - resume()을 사용자 이벤트 핸들러에서 호출해야 함
// - 이 클래스는 getContext() 호출 시 자동으로 resume을 시도
//
// ## 지연 시간(Latency) 고려사항
// - 낮은 지연: 실시간 반응 필요 (악기 앱, 게임)
// - 높은 지연 허용: 안정적인 재생 (음악 스트리밍)
// - baseLatency: 오디오 그래프 처리 지연
// - outputLatency: 스피커 출력까지의 지연
//
// ## 이 파일을 이해한 다음, 이어서 보면 좋은 파일:
// 1. AudioNodeGraph.ts - 오디오 노드 연결 관리
// 2. AudioLoader.ts - 오디오 파일 로딩
// 3. FFTAnalyzer.ts - 주파수 분석
// =============================================================================
