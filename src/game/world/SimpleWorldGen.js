const { Worker, MessageChannel } = require('worker_threads');

module.exports = class SimpleWorldGen {
  constructor() {
    this._worker = new Worker(`${__dirname}/chunk_worker/index.js`, {resourceLimits: {

    }});
    this._worker.on('error', err => {
      console.error(err);
      process.exit(1);
    });

    this._worker.on('message', value => {
      console.log('Worker: ', value);
    });
  }

  generateChunk(x, y, z, chunkDim, cb) {    
    const { port1: sendPort, port2: recvPort } = new MessageChannel();
    this._worker.postMessage({x, y, z, chunkDim, sendPort}, [sendPort]);

    recvPort.once('message', chunk => {
      cb(null, chunk);
      recvPort.close();
    });
  }

}
