/**
 * FFT Analyzer - 주파수 분석
 * @version 3.2.2
 */

export interface FFTData {
  frequencyData: Uint8Array;
  timeDomainData: Uint8Array;
  frequencies: Float32Array;
}

export interface FrequencyBands {
  sub: number;      // 20-60 Hz
  bass: number;     // 60-250 Hz
  lowMid: number;   // 250-500 Hz
  mid: number;      // 500-2000 Hz
  highMid: number;  // 2000-4000 Hz
  high: number;     // 4000-20000 Hz
}

export class FFTAnalyzer {
  private context: AudioContext;
  private analyser: AnalyserNode;
  private frequencyData: Uint8Array<ArrayBuffer>;
  private timeDomainData: Uint8Array<ArrayBuffer>;
  private floatFrequencyData: Float32Array<ArrayBuffer>;

  constructor(
    context: AudioContext,
    fftSize: number = 2048,
    smoothingTimeConstant: number = 0.8
  ) {
    this.context = context;
    this.analyser = context.createAnalyser();
    this.analyser.fftSize = fftSize;
    this.analyser.smoothingTimeConstant = smoothingTimeConstant;

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
