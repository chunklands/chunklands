const { Piscina } = require('piscina')
const { MessageChannel } = require('worker_threads');

module.exports = class SimpleWorldGen {
  constructor(blockIds) {

    this._piscina = new Piscina({
      filename: `${__dirname}/chunk_worker/index.js`,
      workerData: blockIds
    });

    this._piscina.on('error', err => {
      console.error(err);
      process.exit(1);
    });
  }

  generateChunk(x, y, z, chunkDim, cb) {
    const { port1: sendPort, port2: recvPort } = new MessageChannel();
    this._piscina.runTask({x, y, z, chunkDim, sendPort}, [sendPort]);

    recvPort.once('message', chunk => {
      cb(null, chunk);
      recvPort.close();
    });
  }

}
