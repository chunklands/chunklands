import { MessagePort } from 'worker_threads';
import { Math } from '../../../chunklands.node';

export interface IChunk {
  data: ArrayBuffer;
}

export interface IChunkLoader {
  getChunk(x: number, y: number, z: number): IChunk;
}

export default class ChunkPrioLoader {
  private chunks: { pos: Math.Pos3D; sendPort: MessagePort }[] = [];
  private pos: Math.Pos3D = { x: 0, y: 0, z: 0 };
  private generating = false;

  constructor(private chunkLoader: IChunkLoader) {}

  updatePositionAndSort(pos: Math.Pos3D) {
    this.pos = pos;
    this.sortChunks();
  }

  addChunk(pos: Math.Pos3D, sendPort: MessagePort) {
    this.chunks.push({ pos, sendPort });
    // let's not sort here

    this.generateChunks();
  }

  private sortChunks() {
    this.chunks.sort(
      ({ pos: posA }, { pos: posB }) =>
        (posB.x - this.pos.x) ** 2 +
        (posB.y - this.pos.y) ** 2 +
        (posB.z - this.pos.z) ** 2 -
        ((posA.x - this.pos.x) ** 2 +
          (posA.y - this.pos.y) ** 2 +
          (posA.z - this.pos.z) ** 2)
    );
  }

  private generateChunks() {
    if (this.generating) {
      return;
    }

    const startGenerate = () => {
      const item = this.chunks.shift();

      if (item === undefined) {
        this.generating = false;
        return;
      }

      const { pos, sendPort } = item;
      // const camDistance = Math.sqrt(
      //   (pos.x - this.pos.x) ** 2 +
      //     (pos.y - this.pos.y) ** 2 +
      //     (pos.z - this.pos.z) ** 2
      // );
      // console.log(`get chunk: ${camDistance}`);
      const chunk = this.chunkLoader.getChunk(pos.x, pos.y, pos.z);
      sendPort.postMessage(chunk.data);
      sendPort.close();

      setTimeout(startGenerate, 0);
    };

    startGenerate();
  }
}
