const { parentPort, workerData, isMainThread } = require('worker_threads');

if (isMainThread) {
  throw new Error('do not use from main thread');
}

// WorldGenerator:
// 1. ocean / land via simplex noise
// 2. Biomes + HeightMap via voronoi cells
// 3. biome dependant heightMap smoothing
// 4. caves
// 5. multi-blocks

const { chunkDim, blocks } = workerData;
const { ChunkLoader } = require('./chunks')(chunkDim, blocks);

parentPort.on('message', handleMessage);

const chunkLoader = new ChunkLoader();

/**
 * @param {{x: number, y: number, z: number, sendPort: import('worker_threads').MessagePort}} param0 
 */
function handleMessage({x, y, z, sendPort}) {
  // const data = new ArrayBuffer(4 * (16 ** 3));
  // const dataBlocks = new Int32Array(data);
  // if (x === 0 && y === 0 && z === 0) {
  //   dataBlocks.fill(blocks['block.dirt']);
  // } else {
  //   dataBlocks.fill(blocks['block.air']);
  // }
  
  // sendPort.postMessage(data);
  // sendPort.close();

  const chunk = chunkLoader.getChunk(x, y, z);
  
  // we cannot send back a TypedBuffer -.-

  // do not transfer ArrayBuffer, because chunk may be unloaded and reloaded
  // => transferred ArrayBuffer would be empty then
  sendPort.postMessage(chunk.data);
  sendPort.close();
}
