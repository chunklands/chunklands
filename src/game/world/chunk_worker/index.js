const world = require('./world');
const noise = require('../../../noise');
noise.seed(94832281);

const { parentPort } = require('worker_threads');

parentPort.on('message', onMessage);

/**
 * 
 * @param {{x: number, y: number, z: number, chunkDim: number, sendPort: MessagePort}} param0 
 */
function onMessage({x, y, z, chunkDim, sendPort}) {
  const chunk = generateChunk(x, y, z, chunkDim);
  
  sendPort.postMessage(chunk);
  sendPort.close();
}


function generateChunk(x, y, z, chunkDim) {
  const chunk = new Array(chunkDim * chunkDim * chunkDim);

  const px = x * chunkDim;
  const py = y * chunkDim;
  const pz = z * chunkDim;

  let i = 0;
  for (let lz = 0; lz < chunkDim; lz++) {
    for (let ly = 0; ly < chunkDim; ly++) {
      for (let lx = 0; lx < chunkDim; lx++) {
        const ax = px + lx;
        const ay = py + ly;
        const az = pz + lz;

        // if (ay <= 10) {
        //   chunk[i] = 'block.water';
        // } else
        if (isGround(ax, ay, az)) {
          if (ay < 0) {
            chunk[i] = 'block.stone';
          } else {
            chunk[i] = isGround(ax, ay+1, az) ? 'block.dirt' : 'block.grass';
          }
        } else {
          chunk[i] = 'block.air';
        }

        i++;
      }
    }
  }

  return chunk;
}



function isGround(x, y, z) {
  return world.isGround(x, y, z);
}
