/**
 * Script Processor Wrapper - 레거시 처리 지원
 * @version 3.2.2
 * @deprecated ScriptProcessorNode is deprecated, use AudioWorklet
 */

export type ProcessCallback = (
  inputBuffer: AudioBuffer,
  outputBuffer: AudioBuffer
) => void;

export class ScriptProcessorWrapper {
  private processor: ScriptProcessorNode;
  private callback: ProcessCallback | null = null;

  constructor(
    context: AudioContext,
    bufferSize: number = 4096,
    inputChannels: number = 2,
    outputChannels: number = 2
  ) {
    // ScriptProcessorNode is deprecated but still useful for compatibility
    this.processor = context.createScriptProcessor(
      bufferSize,
      inputChannels,
      outputChannels
    );

    this.processor.onaudioprocess = this.handleProcess.bind(this);
  }

  private handleProcess(event: AudioProcessingEvent): void {
    if (this.callback) {
      this.callback(event.inputBuffer, event.outputBuffer);
    } else {
      // Pass through
      for (let channel = 0; channel < event.outputBuffer.numberOfChannels; channel++) {
        const input = event.inputBuffer.getChannelData(channel);
        const output = event.outputBuffer.getChannelData(channel);
        output.set(input);
      }
    }
  }

  setCallback(callback: ProcessCallback): void {
    this.callback = callback;
  }

  getNode(): ScriptProcessorNode {
    return this.processor;
  }

  connect(destination: AudioNode): void {
    this.processor.connect(destination);
  }

  disconnect(): void {
    this.processor.disconnect();
  }

  getBufferSize(): number {
    return this.processor.bufferSize;
  }

  /**
   * 볼륨 증폭 프로세서
   */
  static createGainProcessor(
    context: AudioContext,
    gain: number = 1
  ): ScriptProcessorWrapper {
    const wrapper = new ScriptProcessorWrapper(context);
    wrapper.setCallback((input, output) => {
      for (let ch = 0; ch < output.numberOfChannels; ch++) {
        const inp = input.getChannelData(ch);
        const out = output.getChannelData(ch);
        for (let i = 0; i < out.length; i++) {
          out[i] = inp[i] * gain;
        }
      }
    });
    return wrapper;
  }

  /**
   * 클리핑 프로세서
   */
  static createClipProcessor(
    context: AudioContext,
    threshold: number = 0.9
  ): ScriptProcessorWrapper {
    const wrapper = new ScriptProcessorWrapper(context);
    wrapper.setCallback((input, output) => {
      for (let ch = 0; ch < output.numberOfChannels; ch++) {
        const inp = input.getChannelData(ch);
        const out = output.getChannelData(ch);
        for (let i = 0; i < out.length; i++) {
          out[i] = Math.max(-threshold, Math.min(threshold, inp[i]));
        }
      }
    });
    return wrapper;
  }
}
