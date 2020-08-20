const { MessageChannel, Worker } = require('worker_threads');

module.exports = class SimpleWorldGen {
  /**
   * @param {{[blockId: string]: bigint}} blocks 
   */
  constructor(blocks) {

    this._worker = new Worker(`${__dirname}/chunk_worker/index.js`, {
      workerData: {
        blocks,
        chunkDim: 32 // TODO(daaitch): magic number
      }
    });

    this._openMessagePorts = new Set();

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
   * @param {(err: Error, buf: ArrayBuffer) => void} callback
   */
  generateChunk(x, y, z, chunkDim, callback) {
    const { port1: sendPort, port2: recvPort } = new MessageChannel();
    this._openMessagePorts.add(sendPort).add(recvPort);

    this._worker.postMessage({x, y, z, chunkDim, sendPort}, [sendPort]);

    /**
     * @param {ArrayBuffer} buffer 
     */
    const handleMessage = buffer => {
      if (buffer.byteLength === 0) {
        throw new Error(`no buffer for: ${x} ${y} ${z}`);
      }
      callback(null, buffer);
      recvPort.close();
      sendPort.close();
      this._openMessagePorts.delete(recvPort);
      this._openMessagePorts.delete(sendPort);
    }

    recvPort.once('message', handleMessage);
  }

  async terminate() {
    await this._worker.terminate();
    this._openMessagePorts.forEach(port => {
      port.close();
    });
    this._openMessagePorts.clear();
  }
}
