// [FILE]
// - 목적: 오디오 주파수 분석 (FFT - Fast Fourier Transform)
// - 주요 역할: 시간 영역 → 주파수 영역 변환, 스펙트럼 데이터 제공
// - 관련 클론 가이드 단계: [CG-v3.2.2] 실시간 오디오 처리
// - 권장 읽는 순서: AudioContextManager.ts 이후
//
// [LEARN] C 개발자를 위한 FFT (고속 푸리에 변환):
// - 시간 영역 신호(파형)를 주파수 영역(스펙트럼)으로 변환한다.
// - 어떤 주파수 성분이 얼마나 강한지 알 수 있다.
// - 오디오 시각화(스펙트럼 분석기)의 핵심 기술이다.
// - Web Audio API의 AnalyserNode가 FFT를 내장하고 있다.
//
// [Reader Notes]
// - fftSize: FFT 크기 (2의 거듭제곱, 예: 2048)
// - frequencyBinCount: 주파수 빈 개수 = fftSize / 2
// - 각 빈은 특정 주파수 범위를 나타낸다.
// - 주파수 = (빈 인덱스 * 샘플레이트) / fftSize

/**
 * FFT Analyzer - 주파수 분석
 * @version 3.2.2
 */

// [LEARN] 분석 결과 데이터 구조
export interface FFTData {
  frequencyData: Uint8Array;    // 주파수별 진폭 (0-255)
  timeDomainData: Uint8Array;   // 시간 영역 파형 (0-255)
  frequencies: Float32Array;    // 각 빈의 실제 주파수 (Hz)
}

// [LEARN] 주파수 대역별 분류
// - 사람이 듣는 오디오를 의미 있는 대역으로 나눈다.
// - 이퀄라이저나 음악 시각화에서 많이 사용한다.
export interface FrequencyBands {
  sub: number;      // 20-60 Hz (서브베이스, 느끼는 저음)
  bass: number;     // 60-250 Hz (베이스, 킥드럼)
  lowMid: number;   // 250-500 Hz (저중음)
  mid: number;      // 500-2000 Hz (중음, 보컬 핵심)
  highMid: number;  // 2000-4000 Hz (고중음, 선명함)
  high: number;     // 4000-20000 Hz (고음, 심벌)
}

export class FFTAnalyzer {
  private context: AudioContext;
  private analyser: AnalyserNode;  // Web Audio API FFT 노드
  private frequencyData: Uint8Array<ArrayBuffer>;
  private timeDomainData: Uint8Array<ArrayBuffer>;
  private floatFrequencyData: Float32Array<ArrayBuffer>;

  constructor(
    context: AudioContext,
    fftSize: number = 2048,           // FFT 크기 (해상도)
    smoothingTimeConstant: number = 0.8  // 스무딩 (0-1, 높을수록 부드러움)
  ) {
    this.context = context;
    // [LEARN] AnalyserNode 생성
    // - createAnalyser()로 FFT 분석 노드를 만든다.
    // - 오디오 그래프에 연결하면 실시간 분석이 가능하다.
    this.analyser = context.createAnalyser();
    this.analyser.fftSize = fftSize;
    this.analyser.smoothingTimeConstant = smoothingTimeConstant;

    // [LEARN] 버퍼 할당
    // - frequencyBinCount = fftSize / 2 (나이퀴스트 정리)
    // - Uint8Array: 0-255 범위의 정규화된 값
    const bufferLength = this.analyser.frequencyBinCount;
    this.frequencyData = new Uint8Array(bufferLength) as Uint8Array<ArrayBuffer>;
    this.timeDomainData = new Uint8Array(bufferLength) as Uint8Array<ArrayBuffer>;
    this.floatFrequencyData = new Float32Array(bufferLength) as Float32Array<ArrayBuffer>;
  }

  getNode(): AnalyserNode {
    return this.analyser;
  }

  setFFTSize(size: number): void {
    this.analyser.fftSize = size;
    const bufferLength = this.analyser.frequencyBinCount;
    this.frequencyData = new Uint8Array(bufferLength) as Uint8Array<ArrayBuffer>;
    this.timeDomainData = new Uint8Array(bufferLength) as Uint8Array<ArrayBuffer>;
    this.floatFrequencyData = new Float32Array(bufferLength) as Float32Array<ArrayBuffer>;
  }

  setSmoothing(value: number): void {
    this.analyser.smoothingTimeConstant = Math.max(0, Math.min(1, value));
  }

  // [LEARN] 주파수 데이터 획득
  // - getByteFrequencyData(): 0-255 범위의 Uint8Array
  // - 각 요소가 해당 주파수 빈의 진폭을 나타낸다.
  getFrequencyData(): Uint8Array {
    this.analyser.getByteFrequencyData(this.frequencyData);
    return this.frequencyData;
  }

  getFloatFrequencyData(): Float32Array {
    this.analyser.getFloatFrequencyData(this.floatFrequencyData);
    return this.floatFrequencyData;
  }

  getTimeDomainData(): Uint8Array {
    this.analyser.getByteTimeDomainData(this.timeDomainData);
    return this.timeDomainData;
  }

  getFFTData(): FFTData {
    this.analyser.getByteFrequencyData(this.frequencyData);
    this.analyser.getByteTimeDomainData(this.timeDomainData);

    // Calculate frequency for each bin
    const frequencies = new Float32Array(this.analyser.frequencyBinCount);
    const nyquist = this.context.sampleRate / 2;
    for (let i = 0; i < frequencies.length; i++) {
      frequencies[i] = (i / frequencies.length) * nyquist;
    }

    return {
      frequencyData: this.frequencyData,
      timeDomainData: this.timeDomainData,
      frequencies,
    };
  }

  getFrequencyBands(): FrequencyBands {
    this.analyser.getByteFrequencyData(this.frequencyData);
    
    const nyquist = this.context.sampleRate / 2;
    const binSize = nyquist / this.analyser.frequencyBinCount;

    const getAverageInRange = (minFreq: number, maxFreq: number): number => {
      const startBin = Math.floor(minFreq / binSize);
      const endBin = Math.min(
        Math.ceil(maxFreq / binSize),
        this.frequencyData.length - 1
      );

      let sum = 0;
      let count = 0;
      for (let i = startBin; i <= endBin; i++) {
        sum += this.frequencyData[i];
        count++;
      }

      return count > 0 ? sum / count / 255 : 0;
    };

    return {
      sub: getAverageInRange(20, 60),
      bass: getAverageInRange(60, 250),
      lowMid: getAverageInRange(250, 500),
      mid: getAverageInRange(500, 2000),
      highMid: getAverageInRange(2000, 4000),
      high: getAverageInRange(4000, 20000),
    };
  }

  getPeakFrequency(): { frequency: number; magnitude: number } {
    this.analyser.getByteFrequencyData(this.frequencyData);
    
    let maxValue = 0;
    let maxIndex = 0;
    
    for (let i = 0; i < this.frequencyData.length; i++) {
      if (this.frequencyData[i] > maxValue) {
        maxValue = this.frequencyData[i];
        maxIndex = i;
      }
    }

    const nyquist = this.context.sampleRate / 2;
    const frequency = (maxIndex / this.frequencyData.length) * nyquist;

    return {
      frequency,
      magnitude: maxValue / 255,
    };
  }

  getRMS(): number {
    this.analyser.getByteTimeDomainData(this.timeDomainData);
    
    let sum = 0;
    for (let i = 0; i < this.timeDomainData.length; i++) {
      const normalized = (this.timeDomainData[i] - 128) / 128;
      sum += normalized * normalized;
    }

    return Math.sqrt(sum / this.timeDomainData.length);
  }

  getDecibels(): number {
    const rms = this.getRMS();
    if (rms === 0) return -Infinity;
    return 20 * Math.log10(rms);
  }

  getFrequencyBinCount(): number {
    return this.analyser.frequencyBinCount;
  }

  connect(source: AudioNode): void {
    source.connect(this.analyser);
  }

  disconnect(): void {
    this.analyser.disconnect();
  }
}
