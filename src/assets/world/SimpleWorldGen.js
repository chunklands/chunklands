const { MessageChannel, Worker } = require('worker_threads');
const Abort = require('../../lib/Abort');

module.exports = class SimpleWorldGen {
  /**
   * @param {{abort: Abort, blocks: {[blockId: string]: bigint}}} param0
   */
  constructor({abort, blocks}) {

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
    });

    abort.onceAbort(async () => {
      await this._worker.terminate();      
    });
  }

  /**
   * 
   * @param {import('../../lib/Abort')} abort 
   * @param {{x: number, y: number, z: number}} param1 
   * @param {number} chunkDim
   */
  async generateChunk(abort, {x, y, z}, chunkDim) {
    abort.check();

    const { port1: sendPort, port2: recvPort } = new MessageChannel();

    try {
      this._worker.postMessage({x, y, z, chunkDim, sendPort}, [sendPort]);
      return await receiveBuffer(abort, recvPort);
    } finally {
      sendPort.close(); // is transmitted :-/
      recvPort.close();
    }
  }
}

/**
 * 
 * @param {import('../../lib/Abort')} abort 
 * @param {import('worker_threads').MessagePort} port 
 */
async function receiveBuffer(abort, port) {
  return await new Promise((resolve, reject) => {
    
    const messageCleanup = () => {
      port.off('message', handleMessage);
    }
    
    const abortCleanup = abort.onceAbort(() => {
      messageCleanup();
      reject(Abort.ABORT_ERROR);
    });

    port.once('message', handleMessage);

    function handleMessage(buffer) {
      abortCleanup();

      if (buffer.byteLength === 0) {
        reject(new Error(`no buffer for: ${x} ${y} ${z}`));
        return;
      }

      resolve(buffer);
    }
  });
}