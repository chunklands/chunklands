const { MessageChannel, Worker } = require('worker_threads');

module.exports = class SimpleWorldGen {
  /**
   * @param {number[]} blockIds 
   */
  constructor(blockIds) {

    this._worker = new Worker(`${__dirname}/chunk_worker/index.js`, {
      workerData: blockIds
    });

    this._worker.on('error', err => {
      console.error(err);
      process.exit(1);
    });

    this._worker.on('message', msg => {
      console.log(msg);
    })
  }

  generateChunk(x, y, z, chunkDim, cb) {
    const { port1: sendPort, port2: recvPort } = new MessageChannel();
    this._worker.postMessage({x, y, z, chunkDim, sendPort}, [sendPort])

    recvPort.once('message', chunk => {
      cb(null, chunk);
      recvPort.close();
    });
  }

}
