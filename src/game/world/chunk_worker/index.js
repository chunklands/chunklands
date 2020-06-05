const { parentPort, workerData, isMainThread } = require('worker_threads');

if (isMainThread) {
  throw new Error('is a worker');
}

// WorldGenerator:
// 1. ocean / land via perlin noise
// 2. Biomes + HeightMap via voronoi cells
// 3. biome dependant heightMap smoothing
// 4. multi-blocks

const { chunkDim, blocks } = workerData;
const {
  'block.air': BLOCK_AIR,
  'block.grass': BLOCK_GRASS
} = blocks;

parentPort.on('message', handleMessage);


/**
 * @param {{x: number, y: number, z: number, sendPort: import('worker_threads').MessagePort}} param0 
 */
function handleMessage({x, y, z, sendPort}) {
  const buffer = new ArrayBuffer(4 * (chunkDim ** 3));
  const buf = new Int32Array(buffer);
  generateChunk(buf, x, y, z);
  
  // we cannot send back a TypedBuffer -.-
  sendPort.postMessage(buffer, [buffer]);
  sendPort.close();
}

/**
 * @param {Int32Array} buf 
 * @param {number} chunkX 
 * @param {number} chunkY 
 * @param {number} chunkZ 
 */
function generateChunk(buf, chunkX, chunkY, chunkZ) {
  buf.fill(BLOCK_AIR);

  if (chunkY === 0) {
    for (let x = 0; x < chunkDim; x++) {
      for (let z = 0; z < chunkDim; z++) {
        buf[blockIndex(x, 0, z)] = BLOCK_GRASS;
      }
    }
  }
}

/**
 * @param {number} x 
 * @param {number} y 
 * @param {number} z 
 */
function blockIndex(x, y, z) {
  return x + chunkDim * (y + chunkDim * z);
}