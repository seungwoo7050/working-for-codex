/**
 * Audio Parameters - 파라미터 자동화
 * @version 3.2.1
 */

export type AutomationCurve = 'linear' | 'exponential' | 'instant' | 'target';

export interface AutomationPoint {
  time: number;
  value: number;
  curve?: AutomationCurve;
  timeConstant?: number;
}

export class AudioParameters {
  private context: AudioContext;

  constructor(context: AudioContext) {
    this.context = context;
  }

  /**
   * 즉시 값 설정
   */
  setValue(param: AudioParam, value: number): void {
    param.value = value;
  }

  /**
   * 특정 시간에 값 설정
   */
  setValueAtTime(param: AudioParam, value: number, time: number): void {
    param.setValueAtTime(value, time);
  }

  /**
   * 선형 보간으로 값 변경
   */
  linearRampTo(param: AudioParam, value: number, endTime: number): void {
    param.linearRampToValueAtTime(value, endTime);
  }

  /**
   * 지수 보간으로 값 변경 (0 제외)
   */
  exponentialRampTo(param: AudioParam, value: number, endTime: number): void {
    // Exponential can't reach 0
    const safeValue = Math.max(value, 0.0001);
    param.exponentialRampToValueAtTime(safeValue, endTime);
  }

  /**
   * 타겟 값으로 점진적 변경
   */
  setTargetAtTime(
    param: AudioParam,
    target: number,
    startTime: number,
    timeConstant: number
  ): void {
    param.setTargetAtTime(target, startTime, timeConstant);
  }

  /**
   * 커브 배열로 값 변경
   */
  setValueCurve(
    param: AudioParam,
    values: Float32Array | number[],
    startTime: number,
    duration: number
  ): void {
    const curve = values instanceof Float32Array ? values : new Float32Array(values);
    param.setValueCurveAtTime(curve, startTime, duration);
  }

  /**
   * 스케줄된 변경 취소
   */
  cancelScheduled(param: AudioParam, startTime?: number): void {
    if (startTime !== undefined) {
      param.cancelScheduledValues(startTime);
    } else {
      param.cancelScheduledValues(0);
    }
  }

  /**
   * 현재 값 유지하며 취소
   */
  cancelAndHold(param: AudioParam, holdTime: number): void {
    type ExtendedAudioParam = AudioParam & { cancelAndHoldAtTime?: (time: number) => void };
    const extParam = param as ExtendedAudioParam;
    if (extParam.cancelAndHoldAtTime) {
      extParam.cancelAndHoldAtTime(holdTime);
    } else {
      param.cancelScheduledValues(holdTime);
      param.setValueAtTime(param.value, holdTime);
    }
  }

  /**
   * 자동화 포인트 시퀀스 적용
   */
  applyAutomation(param: AudioParam, points: AutomationPoint[]): void {
    const currentTime = this.context.currentTime;
    
    // Start from current value
    param.setValueAtTime(param.value, currentTime);

    for (const point of points) {
      const time = currentTime + point.time;
      
      switch (point.curve ?? 'linear') {
        case 'instant':
          param.setValueAtTime(point.value, time);
          break;
        case 'linear':
          param.linearRampToValueAtTime(point.value, time);
          break;
        case 'exponential':
          param.exponentialRampToValueAtTime(Math.max(point.value, 0.0001), time);
          break;
        case 'target':
          param.setTargetAtTime(point.value, time, point.timeConstant ?? 0.1);
          break;
      }
    }
  }

  /**
   * 페이드 인
   */
  fadeIn(param: AudioParam, duration: number, maxValue: number = 1): void {
    const now = this.context.currentTime;
    param.setValueAtTime(0, now);
    param.linearRampToValueAtTime(maxValue, now + duration);
  }

  /**
   * 페이드 아웃
   */
  fadeOut(param: AudioParam, duration: number): void {
    const now = this.context.currentTime;
    param.setValueAtTime(param.value, now);
    param.linearRampToValueAtTime(0, now + duration);
  }

  /**
   * 크로스페이드
   */
  crossfade(
    fadeOutParam: AudioParam,
    fadeInParam: AudioParam,
    duration: number
  ): void {
    const now = this.context.currentTime;
    
    fadeOutParam.setValueAtTime(fadeOutParam.value, now);
    fadeOutParam.linearRampToValueAtTime(0, now + duration);
    
    fadeInParam.setValueAtTime(0, now);
    fadeInParam.linearRampToValueAtTime(1, now + duration);
  }

  /**
   * ADSR 엔벨로프
   */
  adsr(
    param: AudioParam,
    attack: number,
    decay: number,
    sustain: number,
    release: number,
    peakValue: number = 1,
    sustainDuration: number = 1
  ): number {
    const now = this.context.currentTime;
    
    param.setValueAtTime(0, now);
    param.linearRampToValueAtTime(peakValue, now + attack);
    param.linearRampToValueAtTime(sustain * peakValue, now + attack + decay);
    param.setValueAtTime(sustain * peakValue, now + attack + decay + sustainDuration);
    param.linearRampToValueAtTime(0, now + attack + decay + sustainDuration + release);

    return attack + decay + sustainDuration + release;
  }

  /**
   * LFO 효과 (저주파 오실레이터)
   */
  createLFO(
    target: AudioParam,
    frequency: number,
    depth: number,
    centerValue: number
  ): { oscillator: OscillatorNode; gain: GainNode; start: () => void; stop: () => void } {
    const osc = this.context.createOscillator();
    const gain = this.context.createGain();

    osc.frequency.value = frequency;
    gain.gain.value = depth;

    osc.connect(gain);
    gain.connect(target);

    // Set center value
    target.setValueAtTime(centerValue, this.context.currentTime);

    return {
      oscillator: osc,
      gain,
      start: () => osc.start(),
      stop: () => osc.stop(),
    };
  }
}
