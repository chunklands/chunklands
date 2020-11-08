export default function createMath(chunkDim: number) {
  function blockIndex3D(x: number, y: number, z: number) {
    return x + chunkDim * (y + chunkDim * z);
  }

  function blockIndex2D(x: number, y: number) {
    return x + chunkDim * y;
  }

  function blockPos2D(blockIndex: number) {
    return [blockIndex % chunkDim, (blockIndex / chunkDim) | 0];
  }

  function centerChunkPos(px: number, pz: number) {
    return [Math.floor(px / chunkDim), Math.floor(pz / chunkDim)];
  }

  return { blockIndex2D, blockIndex3D, blockPos2D, centerChunkPos };
}
