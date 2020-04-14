const cave = require('./cave');
const mountain = require('./mountain');
const noise = require('../../../noise');
noise.seed(94832281);

const { parentPort } = require('worker_threads');

parentPort.on('message', ({x, y, z, chunkDim}) => {
  const chunk = generateChunk(x, y, z, chunkDim);
  
  parentPort.postMessage({x, y, z, chunk});
});


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
            chunk[i] = 'block.sand';
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
  // if (y > 0) {
  //   return isMountainsGround(x, y, z);
  // }

  // if (y > -20) {
  //   // 0; -20
  //   const mountainsFactor = 1 - (y / -20);
  //   const caveFactor = 1 - mountainsFactor;

  //   const m = isMountainsGround(x, y, z) ? 1 : 0;
  //   const c = isCaveGround(x, y, z) ? 1 : 0

  //   return (m * mountainsFactor + c * caveFactor) > 0.5;
  // }

  return cave.isGround(x, y, z);
}


