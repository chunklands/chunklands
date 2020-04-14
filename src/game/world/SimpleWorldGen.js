const { Worker } = require('worker_threads');

module.exports = class SimpleWorldGen {
  constructor() {
    this._worker = new Worker(`${__dirname}/chunk_worker/index.js`);
    this._worker.on('error', err => {
      console.error(err);
      process.exit(1);
    });
    this._cbs = new Map();

    this._worker.on('message', ({x, y, z, chunk}) => {
      const key = posKey(x, y, z);
      const cb = this._cbs.get(key);
      this._cbs.delete(key);

      if (!cb) {
        console.warn('okay ... no cb found');
        return;
      }

      cb(null, chunk);
    });
  }

  generateChunk(x, y, z, chunkDim, cb) {    
    this._worker.postMessage({x, y, z, chunkDim});
    this._cbs.set(posKey(x, y, z), cb);
  }

}

function posKey(x, y, z) {
  return `${x},${y},${z}`;
}