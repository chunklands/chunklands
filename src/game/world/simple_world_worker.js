const {parentPort, isMainThread} = require('worker_threads');

if (!isMainThread) {
  parentPort.on('message', ({chunkPos, chunkDim}) => {
    parentPort.postMessage({
      chunkPos,
      chunk: generateChunk(chunkPos, chunkDim)
    });
  });
} else {
  module.exports = generateChunk;
}

function generateChunk(chunkPos, chunkDim) {
  const chunk = new Array(chunkDim * chunkDim * chunkDim);

  const px = chunkPos.x * chunkDim;
  const py = chunkPos.y * chunkDim;
  const pz = chunkPos.z * chunkDim;

  let i = 0;
  for (let lz = 0; lz < chunkDim; lz++) {
    for (let ly = 0; ly < chunkDim; ly++) {
      for (let lx = 0; lx < chunkDim; lx++) {
        const ax = px + lx;
        const ay = py + ly;
        const az = pz + lz;

        if (ay <= 10) {
          chunk[i] = 'block.water';
        } else if (isGroundMountains(ax, ay, az)) {
          if (ay <= 12) {
            chunk[i] = 'block.sand';
          } else {
            chunk[i] = isGroundMountains(ax, ay+1, az) ? 'block.dirt' : 'block.grass';
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

const Ax      = 24;
const omega_x = (2 * Math.PI) / 51;
const phi_x   = (2 * Math.PI) / 37;
const Az      = 37;
const omega_z = (2 * Math.PI) / 33;
const phi_z   = (2 * Math.PI) / 27;

function isGroundMountains(x, y, z) {
  return y < (
      (Ax * Math.sin(omega_x * x + phi_x))
      + (Az * Math.sin(omega_z * z + phi_z))
  );
}
