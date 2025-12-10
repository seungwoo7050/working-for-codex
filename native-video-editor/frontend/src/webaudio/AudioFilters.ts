/**
 * Audio Filters - 오디오 필터 프리셋
 * @version 3.2.2
 */

export type FilterPreset = 
  | 'lowpass' 
  | 'highpass' 
  | 'bandpass' 
  | 'notch' 
  | 'lowshelf' 
  | 'highshelf' 
  | 'peaking'
  | 'allpass';

export interface FilterConfig {
  type: BiquadFilterType;
  frequency: number;
  Q?: number;
  gain?: number;
}

export interface EQBand {
  filter: BiquadFilterNode;
  frequency: number;
  gain: number;
}

export class AudioFilters {
  private context: AudioContext;

  constructor(context: AudioContext) {
    this.context = context;
  }

  createFilter(config: FilterConfig): BiquadFilterNode {
    const filter = this.context.createBiquadFilter();
    filter.type = config.type;
    filter.frequency.value = config.frequency;
    
    if (config.Q !== undefined) {
      filter.Q.value = config.Q;
    }
    if (config.gain !== undefined) {
      filter.gain.value = config.gain;
    }

    return filter;
  }

  /**
   * 로우패스 필터
   */
  createLowpass(frequency: number, Q: number = 1): BiquadFilterNode {
    return this.createFilter({ type: 'lowpass', frequency, Q });
  }

  /**
   * 하이패스 필터
   */
  createHighpass(frequency: number, Q: number = 1): BiquadFilterNode {
    return this.createFilter({ type: 'highpass', frequency, Q });
  }

  /**
   * 밴드패스 필터
   */
  createBandpass(frequency: number, Q: number = 1): BiquadFilterNode {
    return this.createFilter({ type: 'bandpass', frequency, Q });
  }

  /**
   * 노치 필터 (밴드 리젝트)
   */
  createNotch(frequency: number, Q: number = 10): BiquadFilterNode {
    return this.createFilter({ type: 'notch', frequency, Q });
  }

  /**
   * 로우쉘프 필터
   */
  createLowShelf(frequency: number, gain: number): BiquadFilterNode {
    return this.createFilter({ type: 'lowshelf', frequency, gain });
  }

  /**
   * 하이쉘프 필터
   */
  createHighShelf(frequency: number, gain: number): BiquadFilterNode {
    return this.createFilter({ type: 'highshelf', frequency, gain });
  }

  /**
   * 피킹 EQ 필터
   */
  createPeaking(frequency: number, Q: number, gain: number): BiquadFilterNode {
    return this.createFilter({ type: 'peaking', frequency, Q, gain });
  }

  /**
   * 3밴드 EQ
   */
  create3BandEQ(
    lowGain: number = 0,
    midGain: number = 0,
    highGain: number = 0
  ): EQBand[] {
    return [
      {
        filter: this.createLowShelf(320, lowGain),
        frequency: 320,
        gain: lowGain,
      },
      {
        filter: this.createPeaking(1000, 0.5, midGain),
        frequency: 1000,
        gain: midGain,
      },
      {
        filter: this.createHighShelf(3200, highGain),
        frequency: 3200,
        gain: highGain,
      },
    ];
  }

  /**
   * 그래픽 EQ (10밴드)
   */
  createGraphicEQ(gains: number[] = []): EQBand[] {
    const frequencies = [31, 62, 125, 250, 500, 1000, 2000, 4000, 8000, 16000];
    
    return frequencies.map((freq, i) => {
      const gain = gains[i] ?? 0;
      return {
        filter: this.createPeaking(freq, 1.4, gain),
        frequency: freq,
        gain,
      };
    });
  }

  /**
   * 파라메트릭 EQ
   */
  createParametricEQ(
    bands: Array<{ frequency: number; Q: number; gain: number }>
  ): EQBand[] {
    return bands.map(band => ({
      filter: this.createPeaking(band.frequency, band.Q, band.gain),
      frequency: band.frequency,
      gain: band.gain,
    }));
  }

  /**
   * EQ 체인 연결
   */
  connectEQChain(bands: EQBand[]): {
    input: BiquadFilterNode;
    output: BiquadFilterNode;
  } {
    for (let i = 0; i < bands.length - 1; i++) {
      bands[i].filter.connect(bands[i + 1].filter);
    }

    return {
      input: bands[0].filter,
      output: bands[bands.length - 1].filter,
    };
  }

  /**
   * 필터 주파수 응답 계산
   */
  getFrequencyResponse(
    filter: BiquadFilterNode,
    frequencies: Float32Array
  ): { magnitude: Float32Array; phase: Float32Array } {
    const magnitude = new Float32Array(frequencies.length) as Float32Array<ArrayBuffer>;
    const phase = new Float32Array(frequencies.length) as Float32Array<ArrayBuffer>;
    
    filter.getFrequencyResponse(
      frequencies as Float32Array<ArrayBuffer>, 
      magnitude, 
      phase
    );
    
    return { magnitude, phase };
  }

  /**
   * 테스트용 주파수 배열 생성 (로그 스케일)
   */
  createFrequencyArray(
    minFreq: number = 20,
    maxFreq: number = 20000,
    points: number = 100
  ): Float32Array {
    const frequencies = new Float32Array(points);
    const logMin = Math.log10(minFreq);
    const logMax = Math.log10(maxFreq);
    
    for (let i = 0; i < points; i++) {
      const logFreq = logMin + (i / (points - 1)) * (logMax - logMin);
      frequencies[i] = Math.pow(10, logFreq);
    }
    
    return frequencies;
  }
}
