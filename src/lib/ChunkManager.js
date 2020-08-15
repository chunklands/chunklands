const assert = require('assert');

module.exports = class ChunkManager {
  constructor(api, blocks) {
    this._api = api;
    this._blocks = blocks;
    this._currentChunk = {x: 0, y: 0, z: 0};
    this._api.cameraOn('positionchange', event => {
      const pos = chunkPos(event);
      if (pos.x !== this._currentChunk.x || pos.y !== this._currentChunk.y || pos.z !== this._currentChunk.z) {
        this._currentChunk = pos;
        this._createChunk(pos.x, pos.y, pos.z);
      }
    });
  }

  async _createChunk(x, y, z) {
    const chunk = await this._api.chunkCreate(x, y, z);
    await this._api.chunkUpdate(chunk, createChunk(this._blocks, {x, y, z}));
    await this._api.sceneAddChunk(chunk);
  }
}

function chunkPos(coord) {
  return {
    x: chunkDimPos(coord.x),
    y: chunkDimPos(coord.y),
    z: chunkDimPos(coord.z)
  };
}

function chunkDimPos(dim) {
  return dim >= 0 ? Math.floor(dim / 32) : Math.floor((dim - 1) / 32);
}

function createChunk(blocks, coord) {
  const arrayBuffer = new ArrayBuffer(32 * 32 * 32 * BigUint64Array.BYTES_PER_ELEMENT);
  const buffer = new BigUint64Array(arrayBuffer);

  const dirtHandle = blocks['block.dirt'];
  const airHandle = blocks['block.air'];
  assert(dirtHandle);

  const offset = {x: coord.x * 32, y: coord.y * 32, z: coord.z * 32};

  let i = 0;
  for (let z = 0; z < 32; z++) {
    for (let y = 0; y < 32; y++) {
      for (let x = 0; x < 32; x++, i++) {
        if (offset.y + y === 0) {
          buffer[i] = dirtHandle;
        } else {
          buffer[i] = airHandle;
        }
      }
    }
  }

  return arrayBuffer;
}