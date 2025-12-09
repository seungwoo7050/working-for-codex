/**
 * Custom Audio Nodes - 커스텀 노드 팩토리
 * @version 3.2.1
 */

export interface GainNodeOptions {
  gain?: number;
}

export interface DelayNodeOptions {
  delayTime?: number;
  maxDelayTime?: number;
}

export interface FilterNodeOptions {
  type?: BiquadFilterType;
  frequency?: number;
  Q?: number;
  gain?: number;
}

export interface CompressorNodeOptions {
  threshold?: number;
  knee?: number;
  ratio?: number;
  attack?: number;
  release?: number;
}

export class CustomAudioNodes {
  private context: AudioContext;

  constructor(context: AudioContext) {
    this.context = context;
  }

  createGain(options: GainNodeOptions = {}): GainNode {
    const node = this.context.createGain();
    if (options.gain !== undefined) {
      node.gain.value = options.gain;
    }
    return node;
  }

  createDelay(options: DelayNodeOptions = {}): DelayNode {
    const node = this.context.createDelay(options.maxDelayTime ?? 5);
    if (options.delayTime !== undefined) {
      node.delayTime.value = options.delayTime;
    }
    return node;
  }

  createFilter(options: FilterNodeOptions = {}): BiquadFilterNode {
    const node = this.context.createBiquadFilter();
    node.type = options.type ?? 'lowpass';
    if (options.frequency !== undefined) {
      node.frequency.value = options.frequency;
    }
    if (options.Q !== undefined) {
      node.Q.value = options.Q;
    }
    if (options.gain !== undefined) {
      node.gain.value = options.gain;
    }
    return node;
  }

  createCompressor(options: CompressorNodeOptions = {}): DynamicsCompressorNode {
    const node = this.context.createDynamicsCompressor();
    if (options.threshold !== undefined) {
      node.threshold.value = options.threshold;
    }
    if (options.knee !== undefined) {
      node.knee.value = options.knee;
    }
    if (options.ratio !== undefined) {
      node.ratio.value = options.ratio;
    }
    if (options.attack !== undefined) {
      node.attack.value = options.attack;
    }
    if (options.release !== undefined) {
      node.release.value = options.release;
    }
    return node;
  }

  createPanner(): PannerNode {
    return this.context.createPanner();
  }

  createStereoPanner(pan: number = 0): StereoPannerNode {
    const node = this.context.createStereoPanner();
    node.pan.value = pan;
    return node;
  }

  createConvolver(buffer?: AudioBuffer): ConvolverNode {
    const node = this.context.createConvolver();
    if (buffer) {
      node.buffer = buffer;
    }
    return node;
  }

  createWaveShaper(): WaveShaperNode {
    return this.context.createWaveShaper();
  }

  createAnalyser(fftSize: number = 2048): AnalyserNode {
    const node = this.context.createAnalyser();
    node.fftSize = fftSize;
    return node;
  }

  createOscillator(
    type: OscillatorType = 'sine',
    frequency: number = 440
  ): OscillatorNode {
    const node = this.context.createOscillator();
    node.type = type;
    node.frequency.value = frequency;
    return node;
  }

  createBufferSource(buffer?: AudioBuffer): AudioBufferSourceNode {
    const node = this.context.createBufferSource();
    if (buffer) {
      node.buffer = buffer;
    }
    return node;
  }

  createChannelSplitter(channels: number = 2): ChannelSplitterNode {
    return this.context.createChannelSplitter(channels);
  }

  createChannelMerger(channels: number = 2): ChannelMergerNode {
    return this.context.createChannelMerger(channels);
  }

  createMediaElementSource(element: HTMLMediaElement): MediaElementAudioSourceNode {
    return this.context.createMediaElementSource(element);
  }

  createMediaStreamSource(stream: MediaStream): MediaStreamAudioSourceNode {
    return this.context.createMediaStreamSource(stream);
  }

  createMediaStreamDestination(): MediaStreamAudioDestinationNode {
    return this.context.createMediaStreamDestination();
  }
}
