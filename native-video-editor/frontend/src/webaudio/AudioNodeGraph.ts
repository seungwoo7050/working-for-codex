/**
 * Audio Node Graph - 노드 그래프 관리
 * @version 3.2.1
 */

export interface AudioNodeInfo {
  id: string;
  node: AudioNode;
  type: string;
  inputs: string[];
  outputs: string[];
}

export class AudioNodeGraph {
  private context: AudioContext;
  private nodes: Map<string, AudioNodeInfo> = new Map();
  private connections: Map<string, Set<string>> = new Map();

  constructor(context: AudioContext) {
    this.context = context;
  }

  addNode(id: string, node: AudioNode, type: string): void {
    if (this.nodes.has(id)) {
      throw new Error(`Node with id "${id}" already exists`);
    }

    this.nodes.set(id, {
      id,
      node,
      type,
      inputs: [],
      outputs: [],
    });

    this.connections.set(id, new Set());
  }

  removeNode(id: string): void {
    const info = this.nodes.get(id);
    if (!info) return;

    // Disconnect all
    info.node.disconnect();
    
    // Remove from connections
    this.connections.delete(id);
    for (const conns of this.connections.values()) {
      conns.delete(id);
    }

    // Remove from other nodes' inputs/outputs
    for (const node of this.nodes.values()) {
      node.inputs = node.inputs.filter(i => i !== id);
      node.outputs = node.outputs.filter(o => o !== id);
    }

    this.nodes.delete(id);
  }

  getNode(id: string): AudioNode | null {
    return this.nodes.get(id)?.node ?? null;
  }

  getNodeInfo(id: string): AudioNodeInfo | null {
    return this.nodes.get(id) ?? null;
  }

  connect(sourceId: string, destId: string, outputIndex?: number, inputIndex?: number): void {
    const source = this.nodes.get(sourceId);
    const dest = this.nodes.get(destId);

    if (!source || !dest) {
      throw new Error(`Node not found: ${!source ? sourceId : destId}`);
    }

    source.node.connect(dest.node as AudioNode, outputIndex, inputIndex);
    
    source.outputs.push(destId);
    dest.inputs.push(sourceId);
    
    this.connections.get(sourceId)?.add(destId);
  }

  disconnect(sourceId: string, destId?: string): void {
    const source = this.nodes.get(sourceId);
    if (!source) return;

    if (destId) {
      const dest = this.nodes.get(destId);
      if (dest) {
        source.node.disconnect(dest.node);
        source.outputs = source.outputs.filter(o => o !== destId);
        dest.inputs = dest.inputs.filter(i => i !== sourceId);
        this.connections.get(sourceId)?.delete(destId);
      }
    } else {
      source.node.disconnect();
      
      for (const outId of source.outputs) {
        const out = this.nodes.get(outId);
        if (out) {
          out.inputs = out.inputs.filter(i => i !== sourceId);
        }
      }
      
      source.outputs = [];
      this.connections.get(sourceId)?.clear();
    }
  }

  connectToDestination(sourceId: string): void {
    const source = this.nodes.get(sourceId);
    if (!source) {
      throw new Error(`Node not found: ${sourceId}`);
    }

    source.node.connect(this.context.destination);
    source.outputs.push('destination');
  }

  getConnectedNodes(id: string): string[] {
    return Array.from(this.connections.get(id) ?? []);
  }

  getInputNodes(id: string): string[] {
    return this.nodes.get(id)?.inputs ?? [];
  }

  getOutputNodes(id: string): string[] {
    return this.nodes.get(id)?.outputs ?? [];
  }

  getAllNodes(): AudioNodeInfo[] {
    return Array.from(this.nodes.values());
  }

  getNodeCount(): number {
    return this.nodes.size;
  }

  clear(): void {
    for (const info of this.nodes.values()) {
      info.node.disconnect();
    }
    this.nodes.clear();
    this.connections.clear();
  }

  toJSON(): object {
    const nodes = Array.from(this.nodes.values()).map(n => ({
      id: n.id,
      type: n.type,
      inputs: n.inputs,
      outputs: n.outputs,
    }));

    return { nodes };
  }
}
