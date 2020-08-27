const {MessageChannel, Worker} = require('worker_threads');
const Abort = require('../../lib/Abort');

module.exports = class SimpleWorldGen {
  /**
   * @param {{abort: Abort, blocks: {[blockId: string]: bigint}}} param0
   */
  constructor({abort, blocks}) {
    this._worker = new Worker(`${__dirname}/chunk_worker/index.js`, {
      workerData: {
        blocks,
        chunkDim: 32  // TODO(daaitch): magic number
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
   * @param {{x: number, y: number, z: number}} pos
   * @param {number} chunkDim
   */
  async generateChunk(abort, pos) {
    abort.check();

    const {port1: sendPort, port2: recvPort} = new MessageChannel();

    try {
      this._worker.postMessage(
          {type: 'requestchunk', payload: {pos, sendPort}}, [sendPort]);

      return await new Promise((resolve, reject) => {
        const messageCleanup =
            () => {
              recvPort.off('message', handleMessage);
            }

        const abortCleanup = abort.onceAbort(() => {
          messageCleanup();
          reject(Abort.ABORT_ERROR);
        });

        recvPort.once('message', handleMessage);

        function handleMessage(buffer) {
          abortCleanup();

          if (buffer.byteLength === 0) {
            reject(new Error(`no buffer for: ${x} ${y} ${z}`));
            return;
          }

          resolve(buffer);
        }
      });
    } finally {
      sendPort.close();
      recvPort.close();
    }
  }

  updatePosition(pos) {
    this._worker.postMessage({type: 'updateposition', payload: pos});
  }
}

/**
 *
 * @param {import('../../lib/Abort')} abort
 * @param {import('worker_threads').MessagePort} port
 */
async function fetchChunk(abort, pos, port) {}