const algorithm = require('./algorithm');
const math = require('./math');

const oceanNoise = algorithm.createSimplexNoise({f0: 128, octaves: 8, persistence: 0.9, lacunarity: 4.178});

function create(chunkDim) {

  const { blockIndex2D } = math(chunkDim);

  /**
   * @param {Int8Array} oceanMap 
   * @param {number} xOffset 
   * @param {number} zOffset 
   */
  function generateOceanMapForChunk(oceanMap, xOffset, zOffset) {
    for (let px = 0; px < chunkDim; px++) {
      for (let pz = 0; pz < chunkDim; pz++) {
        const sample = oceanNoise(xOffset + px, zOffset + pz);
        oceanMap[blockIndex2D(px, pz)] = sample > 0.5 ? 1 : 0;
      }
    }
  }

  return {
    generateOceanMapForChunk
  };
};

module.exports = create;
