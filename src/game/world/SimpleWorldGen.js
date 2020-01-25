

module.exports = class SimpleWorldGen {
  generateChunk(x, y, z, chunkDim) {
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

          if (isGroundMountains(ax, ay, az)) {
            chunk[i] = isGroundMountains(ax, ay+1, az) ? 'block.dirt' : 'block.grass';
          } else {
            chunk[i] = 'block.air';
          }

          i++;
        }
      }
    }

    return chunk;
    
  }
};


const Ax      = 12;
const omega_x = (2 * Math.PI) / 31;
const phi_x   = (2 * Math.PI) / 10;
const Az      = 9;
const omega_z = (2 * Math.PI) / 44;
const phi_z   = (2 * Math.PI) / 27;

function isGroundMountains(x, y, z) {
  return y < (
      (Ax * Math.sin(omega_x * x + phi_x))
      + (Az * Math.sin(omega_z * z + phi_z))
  );
}