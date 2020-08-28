
module.exports = class ChunkPrioLoader {
  constructor(chunkLoader) {
    this._chunkLoader = chunkLoader;
    this._chunks = [];
    this._pos = {x: 0, y: 0, z: 0};

    this._generating = false;
  }

  updatePositionAndSort(pos) {
    this._pos = pos;
    this._sortChunks();
  }

  addChunk(pos, sendPort) {
    this._chunks.push({pos, sendPort});
    // let's not sort here

    this._generateChunks();
  }

  _sortChunks() {
    this._chunks.sort(
        (a, b) => {((b.x - this._pos.x) ** 2 + (b.y - this._pos.y) ** 2 +
                    (b.z - this._pos.z) ** 2) -
                   (a.x - this._pos.x) ** 2 + (a.y - this._pos.y) ** 2 +
                   (a.z - this._pos.z) ** 2});
  }

  _generateChunks() {
    if (this._generating) {
      return;
    }

    const startGenerate =
        () => {
          if (this._chunks.length === 0) {
            this._generating = false;
            return;
          }

          const {pos, sendPort} = this._chunks.shift();
          const camDistance = Math.sqrt(
              (pos.x - this._pos.x) ** 2, (pos.y - this._pos.y) ** 2,
              (pos.z - this._pos.z) ** 2);
          // console.log(`get chunk: ${camDistance}`);
          const chunk = this._chunkLoader.getChunk(pos.x, pos.y, pos.z);
          sendPort.postMessage(chunk.data);
          sendPort.close();

          setTimeout(startGenerate, 0);
        }

    startGenerate();
  }
}