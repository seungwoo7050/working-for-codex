/**
 * Audio Node Pool - 오디오 노드 풀링
 * @version 3.2.4
 */

export type NodeFactory<T extends AudioNode> = (context: AudioContext) => T;
export type NodeResetter<T extends AudioNode> = (node: T) => void;

export interface PooledNode<T extends AudioNode> {
  node: T;
  inUse: boolean;
  lastUsed: number;
}

export class AudioNodePool<T extends AudioNode> {
  private context: AudioContext;
  private pool: PooledNode<T>[] = [];
  private factory: NodeFactory<T>;
  private resetter: NodeResetter<T>;
  private maxSize: number;

  constructor(
    context: AudioContext,
    factory: NodeFactory<T>,
    resetter: NodeResetter<T>,
    maxSize: number = 20
  ) {
    this.context = context;
    this.factory = factory;
    this.resetter = resetter;
    this.maxSize = maxSize;
  }

  acquire(): T {
    // Find available node
    const available = this.pool.find(pn => !pn.inUse);
    if (available) {
      available.inUse = true;
      available.lastUsed = Date.now();
      this.resetter(available.node);
      return available.node;
    }

    // Create new node
    const node = this.factory(this.context);
    
    if (this.pool.length < this.maxSize) {
      this.pool.push({
        node,
        inUse: true,
        lastUsed: Date.now(),
      });
    }

    return node;
  }

  release(node: T): void {
    const pooled = this.pool.find(pn => pn.node === node);
    if (pooled) {
      pooled.inUse = false;
      pooled.lastUsed = Date.now();
      
      // Disconnect the node
      try {
        node.disconnect();
      } catch {
        // Already disconnected
      }
    }
  }

  getStats(): { total: number; inUse: number; available: number } {
    const inUse = this.pool.filter(pn => pn.inUse).length;
    return {
      total: this.pool.length,
      inUse,
      available: this.pool.length - inUse,
    };
  }

  clear(): void {
    for (const pooled of this.pool) {
      try {
        pooled.node.disconnect();
      } catch {
        // Ignore
      }
    }
    this.pool = [];
  }
}

/**
 * 다양한 노드 타입을 위한 통합 풀 관리자
 */
export class AudioNodePoolManager {
  private gainPool: AudioNodePool<GainNode>;
  private filterPool: AudioNodePool<BiquadFilterNode>;
  private delayPool: AudioNodePool<DelayNode>;
  private pannerPool: AudioNodePool<StereoPannerNode>;

  constructor(context: AudioContext) {
    this.gainPool = new AudioNodePool(
      context,
      ctx => ctx.createGain(),
      node => { node.gain.value = 1; }
    );

    this.filterPool = new AudioNodePool(
      context,
      ctx => ctx.createBiquadFilter(),
      node => {
        node.type = 'lowpass';
        node.frequency.value = 350;
        node.Q.value = 1;
        node.gain.value = 0;
      }
    );

    this.delayPool = new AudioNodePool(
      context,
      ctx => ctx.createDelay(5),
      node => { node.delayTime.value = 0; }
    );

    this.pannerPool = new AudioNodePool(
      context,
      ctx => ctx.createStereoPanner(),
      node => { node.pan.value = 0; }
    );
  }

  acquireGain(): GainNode {
    return this.gainPool.acquire();
  }

  releaseGain(node: GainNode): void {
    this.gainPool.release(node);
  }

  acquireFilter(): BiquadFilterNode {
    return this.filterPool.acquire();
  }

  releaseFilter(node: BiquadFilterNode): void {
    this.filterPool.release(node);
  }

  acquireDelay(): DelayNode {
    return this.delayPool.acquire();
  }

  releaseDelay(node: DelayNode): void {
    this.delayPool.release(node);
  }

  acquirePanner(): StereoPannerNode {
    return this.pannerPool.acquire();
  }

  releasePanner(node: StereoPannerNode): void {
    this.pannerPool.release(node);
  }

  getStats(): Record<string, { total: number; inUse: number; available: number }> {
    return {
      gain: this.gainPool.getStats(),
      filter: this.filterPool.getStats(),
      delay: this.delayPool.getStats(),
      panner: this.pannerPool.getStats(),
    };
  }

  clear(): void {
    this.gainPool.clear();
    this.filterPool.clear();
    this.delayPool.clear();
    this.pannerPool.clear();
  }
}
