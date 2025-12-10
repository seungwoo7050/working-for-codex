/**
 * Audio Worklet Manager - AudioWorklet 관리
 * @version 3.2.2
 */

export interface WorkletProcessorInfo {
  name: string;
  loaded: boolean;
  moduleUrl: string;
}

export class AudioWorkletManager {
  private context: AudioContext;
  private loadedProcessors: Map<string, WorkletProcessorInfo> = new Map();
  private isSupported: boolean;

  constructor(context: AudioContext) {
    this.context = context;
    this.isSupported = 'audioWorklet' in context;
  }

  checkSupport(): boolean {
    return this.isSupported;
  }

  async loadProcessor(name: string, moduleUrl: string): Promise<boolean> {
    if (!this.isSupported) {
      console.warn('AudioWorklet not supported');
      return false;
    }

    if (this.loadedProcessors.has(name)) {
      return true;
    }

    try {
      await this.context.audioWorklet.addModule(moduleUrl);
      
      this.loadedProcessors.set(name, {
        name,
        loaded: true,
        moduleUrl,
      });

      return true;
    } catch (error) {
      console.error(`Failed to load AudioWorklet processor: ${name}`, error);
      return false;
    }
  }

  async loadProcessorFromCode(name: string, code: string): Promise<boolean> {
    if (!this.isSupported) {
      console.warn('AudioWorklet not supported');
      return false;
    }

    const blob = new Blob([code], { type: 'application/javascript' });
    const url = URL.createObjectURL(blob);

    try {
      const result = await this.loadProcessor(name, url);
      URL.revokeObjectURL(url);
      return result;
    } catch (error) {
      URL.revokeObjectURL(url);
      throw error;
    }
  }

  createNode(
    processorName: string,
    options?: AudioWorkletNodeOptions
  ): AudioWorkletNode | null {
    if (!this.isSupported) {
      return null;
    }

    if (!this.loadedProcessors.has(processorName)) {
      console.warn(`Processor "${processorName}" not loaded`);
      return null;
    }

    return new AudioWorkletNode(this.context, processorName, options);
  }

  isProcessorLoaded(name: string): boolean {
    return this.loadedProcessors.get(name)?.loaded ?? false;
  }

  getLoadedProcessors(): string[] {
    return Array.from(this.loadedProcessors.keys());
  }

  /**
   * 기본 게인 프로세서 코드 생성
   */
  static getGainProcessorCode(): string {
    return `
      class GainProcessor extends AudioWorkletProcessor {
        static get parameterDescriptors() {
          return [{
            name: 'gain',
            defaultValue: 1,
            minValue: 0,
            maxValue: 10,
            automationRate: 'a-rate'
          }];
        }

        process(inputs, outputs, parameters) {
          const input = inputs[0];
          const output = outputs[0];
          const gain = parameters.gain;

          for (let channel = 0; channel < output.length; channel++) {
            const inputChannel = input[channel] || new Float32Array(128);
            const outputChannel = output[channel];

            if (gain.length === 1) {
              for (let i = 0; i < outputChannel.length; i++) {
                outputChannel[i] = inputChannel[i] * gain[0];
              }
            } else {
              for (let i = 0; i < outputChannel.length; i++) {
                outputChannel[i] = inputChannel[i] * gain[i];
              }
            }
          }

          return true;
        }
      }

      registerProcessor('gain-processor', GainProcessor);
    `;
  }

  /**
   * 비트크러셔 프로세서 코드 생성
   */
  static getBitCrusherCode(): string {
    return `
      class BitCrusherProcessor extends AudioWorkletProcessor {
        static get parameterDescriptors() {
          return [
            { name: 'bitDepth', defaultValue: 8, minValue: 1, maxValue: 16 },
            { name: 'sampleRateReduction', defaultValue: 1, minValue: 1, maxValue: 32 }
          ];
        }

        constructor() {
          super();
          this.phase = 0;
          this.lastSample = 0;
        }

        process(inputs, outputs, parameters) {
          const input = inputs[0];
          const output = outputs[0];
          const bitDepth = parameters.bitDepth[0];
          const reduction = parameters.sampleRateReduction[0];

          const step = Math.pow(0.5, bitDepth);

          for (let channel = 0; channel < output.length; channel++) {
            const inputChannel = input[channel] || new Float32Array(128);
            const outputChannel = output[channel];

            for (let i = 0; i < outputChannel.length; i++) {
              this.phase += 1;
              if (this.phase >= reduction) {
                this.phase = 0;
                this.lastSample = step * Math.floor(inputChannel[i] / step + 0.5);
              }
              outputChannel[i] = this.lastSample;
            }
          }

          return true;
        }
      }

      registerProcessor('bit-crusher-processor', BitCrusherProcessor);
    `;
  }
}
