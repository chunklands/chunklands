
/**
 * @param {number} chunkDim 
 */
function create(chunkDim) {
  /**
   * @param {number} x 
   * @param {number} y 
   * @param {number} z 
   */
  function blockIndex3D(x, y, z) {
    return x + chunkDim * (y + chunkDim * z);
  }

  /**
   * @param {number} x 
   * @param {number} y 
   */
  function blockIndex2D(x, y) {
    return x + chunkDim * y;
  }

  /**
   * @param {number} blockIndex 
   */
  function blockPos2D(blockIndex) {
    return [blockIndex % chunkDim, (blockIndex / chunkDim)|0];
  }

  function centerChunkPos(px, pz) {
    return [Math.floor(px / chunkDim), Math.floor(pz / chunkDim)];
  }

  return {
    blockIndex2D,
    blockIndex3D,
    blockPos2D,
    centerChunkPos
  };
};

module.exports = create;