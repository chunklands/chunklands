const {parentPort, workerData, isMainThread} = require('worker_threads');

if (isMainThread) {
  throw new Error('do not use from main thread');
}

// WorldGenerator:
// 1. ocean / land via simplex noise
// 2. Biomes + HeightMap via voronoi cells
// 3. biome dependant heightMap smoothing
// 4. caves
// 5. multi-blocks

const {chunkDim, blocks} = workerData;
const {ChunkLoader} = require('./chunks')(chunkDim, blocks);
const ChunkPrioLoader = require('./ChunkPrioLoader');

parentPort.on('message', handleMessage);

const prioLoader = new ChunkPrioLoader(new ChunkLoader());

function handleMessage({type, payload}) {
  if (type === 'updateposition') {
    handleUpdatePosition(payload);
  } else if (type === 'requestchunk') {
    handleRequestChunk(payload);
  }
}

function handleUpdatePosition(pos) {
  prioLoader.updatePositionAndSort(pos);
}

function handleRequestChunk({pos, sendPort}) {
  prioLoader.addChunk(pos, sendPort);
}
