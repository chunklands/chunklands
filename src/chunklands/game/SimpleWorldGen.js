const { MessageChannel, Worker } = require('worker_threads');

module.exports = class SimpleWorldGen {
  /**
   * @param {{[blockId: string]: number}} blocks 
   */
  constructor(blocks) {

    this._worker = new Worker(`${__dirname}/chunk_worker/index.js`, {
      workerData: {
        blocks,
        chunkDim: 32 // TODO(daaitch): magic number
      }
    });

    this._worker.on('error', err => {
      console.error(err);
      process.exit(1);
    });

    this._worker.on('message', msg => {
      console.log(msg);
    })
  }

  /**
   * @param {number} x 
   * @param {number} y 
   * @param {number} z 
   * @param {(err: Error, buf: Int32Array) => void} callback
   */
  generateChunk(x, y, z, chunkDim, callback) {
    const { port1: sendPort, port2: recvPort } = new MessageChannel();
    this._worker.postMessage({x, y, z, chunkDim, sendPort}, [sendPort])
    recvPort.once('message', handleMessage);

    /**
     * @param {ArrayBuffer} buffer 
     */
    function handleMessage(buffer) {
      if (buffer.byteLength === 0) {
        throw new Error(`no buffer for: ${x} ${y} ${z}`);
      }
      callback(null, new Int32Array(buffer));
      recvPort.close();
    }
  }

}
