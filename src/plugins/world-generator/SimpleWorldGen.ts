import { MessageChannel, Worker } from 'worker_threads';
import { Math } from '../../chunklands.node';
import Abort from '../../lib/Abort';

import { filename } from './chunk_worker';

export default class SimpleWorldGen {
  private worker: Worker;

  constructor(blocks: { [blockId: string]: bigint }, abort: Abort) {
    this.worker = new Worker(filename, {
      workerData: {
        blocks,
        chunkDim: 32, // TODO(daaitch): magic number
      },
    });

    this.worker.on('error', (err) => {
      console.error(err);
      process.exit(1);
    });

    this.worker.on('message', (msg) => {
      console.log(msg);
    });

    abort.onceAbort(async () => {
      await this.worker.terminate();
    });
  }

  async generateChunk(pos: Math.Pos3D, abort: Abort) {
    abort.check();

    const { port1: sendPort, port2: recvPort } = new MessageChannel();

    try {
      this.worker.postMessage(
        { type: 'requestchunk', payload: { pos, sendPort } },
        [sendPort]
      );

      return await new Promise<ArrayBuffer>((resolve, reject) => {
        const messageCleanup = () => {
          recvPort.off('message', handleMessage);
        };

        const abortCleanup = abort.onceAbort(() => {
          messageCleanup();
          reject(Abort.ABORT_ERROR);
        });

        recvPort.once('message', handleMessage);

        function handleMessage(buffer: ArrayBuffer) {
          abortCleanup();

          if (buffer.byteLength === 0) {
            reject(new Error(`no buffer for: ${pos.x} ${pos.y} ${pos.z}`));
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

  updatePosition(pos: Math.Pos3D) {
    this.worker.postMessage({ type: 'updateposition', payload: pos });
  }
}
