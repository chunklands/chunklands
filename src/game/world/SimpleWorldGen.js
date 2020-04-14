const noise = require('../../noise');

module.exports = class SimpleWorldGen {
  constructor() {

  }

  generateChunk(x, y, z, chunkDim, cb) {
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
          if (this.isGround(ax, ay, az)) {
            if (ay < 0) {
              chunk[i] = 'block.sand';
            } else {
              chunk[i] = this.isGround(ax, ay+1, az) ? 'block.dirt' : 'block.grass';
            }
          } else {
            chunk[i] = 'block.air';
          }

          i++;
        }
      }
    }

    cb(null, chunk);
  }

  

  isGround(x, y, z) {
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

    return this.isCaveGround(x, y, z);
  }

  isCaveGround(x, y, z) {
    const v = noise.perlin3(x / 40, y / 20, z / 40);
    return v * 200 > y;
  }
}

const Ax      = 24;
const omega_x = (2 * Math.PI) / 51;
const phi_x   = (2 * Math.PI) / 37;
const Az      = 37;
const omega_z = (2 * Math.PI) / 33;
const phi_z   = (2 * Math.PI) / 27;

function isMountainsGround(x, y, z) {
  return y < (
      (Ax * Math.sin(omega_x * x + phi_x))
      + (Az * Math.sin(omega_z * z + phi_z))
  );
}