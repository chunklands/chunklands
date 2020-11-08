import assert from 'assert'
import Abort from '../../lib/Abort';
import { EnginePlugin } from '../engine';
import { Math } from '../../chunklands.node';
import SimpleWorldGen from './SimpleWorldGen';

const RENDER_DISTANCE = 10;
const renderChunkOffsets = generatePosOffsets(RENDER_DISTANCE);
console.log(`renderChunkOffsets: ${renderChunkOffsets.length}`)

export default class ChunkManager {

  private chunks = new Map()
  private currentChunkPos: Math.Pos3D = {x: NaN, y: NaN, z: NaN}
  private worldGen: SimpleWorldGen

  constructor(private engine: EnginePlugin, private blocks, abort: Abort) {
    this.worldGen = new SimpleWorldGen(blocks, abort);
  }

  updatePosition(cameraPos: Math.Pos3D, abort: Abort) {
    const pos = chunkPos(cameraPos);
    if (pos.x !== this.currentChunkPos.x ||
        pos.y !== this.currentChunkPos.y ||
        pos.z !== this.currentChunkPos.z) {
      this.currentChunkPos = pos;
      this._updateWorkerPosition(pos);
      this._handleChunkChange(pos, abort).catch(Abort.catchResolver);
    }
  }

  _updateWorkerPosition(pos: Math.Pos3D) {
    this.worldGen.updatePosition(pos);
  }

  async _handleChunkChange(chunkPos: Math.Pos3D, abort: Abort) {
    abort.check();

    for (const [key, chunk] of this.chunks.entries()) {
      if (chunk.hChunk &&
          distanceCmp(chunk.pos, chunkPos, RENDER_DISTANCE) === 1) {
        const deleted = this.chunks.delete(key);
        assert(deleted);
        this.engine.chunkDelete(chunk.hChunk);
      }
    }

    for (const renderChunkOffset of renderChunkOffsets) {
      const cx = renderChunkOffset.x + chunkPos.x;
      const cy = renderChunkOffset.y + chunkPos.y;
      const cz = renderChunkOffset.z + chunkPos.z;

      const key = `${cx}:${cy}:${cz}`;
      if (!this.chunks.has(key)) {
        const pos = {x: cx, y: cy, z: cz};

        // reserve
        this.chunks.set(key, {pos, hChunk: undefined});

        const hChunk = await this.createChunk(pos, abort);

        // check chunkInfo still active
        const chunkInfo = this.chunks.get(key);
        if (chunkInfo) {
          chunkInfo.hChunk = hChunk;
          this.engine.sceneAddChunk(hChunk);
        } else {
          this.engine.chunkDelete(hChunk);
        }
      }
    }
  }

  async createChunk(chunkPos: Math.Pos3D, abort: Abort) {
    const [handle, buf] = await Promise.all([
      abort.race(this.engine.chunkCreate(chunkPos.x, chunkPos.y, chunkPos.z)),
      abort.race(this.worldGen.generateChunk(chunkPos, abort))
    ]);

    await abort.race(this.engine.chunkUpdate(handle, buf));

    return handle;
  }
}

function distanceCmp(a: Math.Pos3D, b: Math.Pos3D, d: number) {
  const d2 = d ** 2;
  const diff2 = (a.x - b.x) ** 2 + (a.y - b.y) ** 2 + (a.z - b.z) ** 2;
  return diff2 < d2 ? -1 : (diff2 > d2 ? 1 : 0);
}

function chunkPos(coord: Math.Pos3D) {
  return {
    x: chunkDimPos(coord.x),
    y: chunkDimPos(coord.y),
    z: chunkDimPos(coord.z)
  };
}

function chunkDimPos(dim: number) {
  return dim >= 0 ? Math.floor(dim / 32) : Math.floor((dim - 1) / 32);
}

function generatePosOffsets(distance: number) {
  const distance2 = distance ** 2;
  const posWithSquareDistances = [];
  for (let z = -distance; z <= distance; z++) {
    for (let y = -distance; y <= distance; y++) {
      for (let x = -distance; x <= distance; x++) {
        const dist2 = x ** 2 + y ** 6 + z ** 2;  // x^2 + y^6 + z^2
        if (dist2 <= distance2) {
          posWithSquareDistances.push({dist2, pos: {x, y, z}});
        }
      }
    }
  }

  posWithSquareDistances.sort((b, a) => {
    return a.dist2 < b.dist2 ? 1 : (a.dist2 > b.dist2 ? -1 : 0);
  });

  return posWithSquareDistances.map(x => x.pos);
}