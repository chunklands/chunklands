const world = require('./world');
const noise = require('../../../noise');
noise.seed(94832281);

const { parentPort, workerData } = require('worker_threads');

const {
  'block.stone': BLOCK_STONE,
  'block.grass': BLOCK_GRASS,
  'block.dirt': BLOCK_DIRT,
  'block.air': BLOCK_AIR
} = workerData;

if (BLOCK_STONE === undefined || BLOCK_GRASS === undefined || BLOCK_DIRT === undefined || BLOCK_AIR === undefined) {
  throw new TypeError(`bad workerData: ${JSON.stringify(workerData)}`);
}

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
            chunk[i] = BLOCK_STONE;
          } else {
            chunk[i] = isGround(ax, ay+1, az) ? BLOCK_DIRT : BLOCK_GRASS;
          }
        } else {
          chunk[i] = BLOCK_AIR;
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
