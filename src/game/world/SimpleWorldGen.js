const {Worker} = require('worker_threads');
const {EventEmitter} = require('events');

const USE_WORKER = true;

if (USE_WORKER) {
  module.exports = class SimpleWorldGen {
    constructor() {
      this._ee = new EventEmitter();
      this._worker = new Worker(`${__dirname}/simple_world_worker.js`);

      this._worker.on('message', ({chunkPos: {x, y, z}, chunk}) => {
        this._ee.emit(`${x}_${y}_${z}`, chunk);
      });
    }

    generateChunk(x, y, z, chunkDim, cb) {
      this._worker.postMessage({chunkPos: {x, y, z}, chunkDim});
      this._ee.once(`${x}_${y}_${z}`, chunk => {
        cb(undefined, chunk);
      });
    }
  };
} else {
  const generateChunk = require('./simple_world_worker')
  module.exports = class SimpleWorldGen {
    generateChunk(x, y, z, chunkDim, cb) {
      cb(null, generateChunk({x, y, z}, chunkDim));
    }
  };
}
