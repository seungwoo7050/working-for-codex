/**
 * Audio Connector - 노드 연결 유틸리티
 * @version 3.2.1
 */

export type ConnectionTarget = AudioNode | AudioParam;

export interface ConnectionOptions {
  outputIndex?: number;
  inputIndex?: number;
}

export class AudioConnector {
  /**
   * 시리얼 체인 연결 (A -> B -> C)
   */
  static chain(...nodes: AudioNode[]): void {
    for (let i = 0; i < nodes.length - 1; i++) {
      nodes[i].connect(nodes[i + 1]);
    }
  }

  /**
   * 병렬 연결 (source -> [A, B, C] -> destination)
   */
  static parallel(
    source: AudioNode,
    parallels: AudioNode[],
    destination: AudioNode
  ): void {
    for (const node of parallels) {
      source.connect(node);
      node.connect(destination);
    }
  }

  /**
   * 믹스 연결 (multiple sources -> gain -> destination)
   */
  static mix(
    sources: AudioNode[],
    mixer: GainNode,
    destination: AudioNode
  ): void {
    for (const source of sources) {
      source.connect(mixer);
    }
    mixer.connect(destination);
  }

  /**
   * 사이드체인 연결
   */
  static sidechain(
    mainSource: AudioNode,
    sideSource: AudioNode,
    compressor: DynamicsCompressorNode,
    destination: AudioNode
  ): void {
    mainSource.connect(compressor);
    sideSource.connect(compressor);
    compressor.connect(destination);
  }

  /**
   * 센드/리턴 효과
   */
  static sendReturn(
    source: AudioNode,
    effectChain: AudioNode[],
    dryGain: GainNode,
    wetGain: GainNode,
    destination: AudioNode
  ): void {
    // Dry path
    source.connect(dryGain);
    dryGain.connect(destination);

    // Wet path
    source.connect(effectChain[0]);
    this.chain(...effectChain);
    effectChain[effectChain.length - 1].connect(wetGain);
    wetGain.connect(destination);
  }

  /**
   * AudioParam 모듈레이션
   */
  static modulate(
    source: AudioNode,
    target: AudioParam,
    amount?: GainNode
  ): void {
    if (amount) {
      source.connect(amount);
      amount.connect(target);
    } else {
      source.connect(target);
    }
  }

  /**
   * 스테레오 분리 및 처리
   */
  static stereoProcess(
    source: AudioNode,
    leftProcessor: AudioNode,
    rightProcessor: AudioNode,
    destination: AudioNode,
    context: AudioContext
  ): { splitter: ChannelSplitterNode; merger: ChannelMergerNode } {
    const splitter = context.createChannelSplitter(2);
    const merger = context.createChannelMerger(2);

    source.connect(splitter);
    splitter.connect(leftProcessor, 0);
    splitter.connect(rightProcessor, 1);
    leftProcessor.connect(merger, 0, 0);
    rightProcessor.connect(merger, 0, 1);
    merger.connect(destination);

    return { splitter, merger };
  }

  /**
   * 모든 연결 해제
   */
  static disconnectAll(node: AudioNode): void {
    node.disconnect();
  }

  /**
   * 특정 대상과의 연결 해제
   */
  static disconnectFrom(source: AudioNode, target: ConnectionTarget): void {
    if (target instanceof AudioNode) {
      source.disconnect(target);
    } else if (target instanceof AudioParam) {
      source.disconnect(target);
    }
  }
}
