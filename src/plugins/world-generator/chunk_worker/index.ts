import {parentPort, workerData, isMainThread} from 'worker_threads';
import chunks from './chunks'
import ChunkPrioLoader from './ChunkPrioLoader'

export const filename = __filename;

if (!isMainThread) {

  // WorldGenerator:
  // 1. ocean / land via simplex noise
  // 2. Biomes + HeightMap via voronoi cells
  // 3. biome dependant heightMap smoothing
  // 4. caves
  // 5. multi-blocks

  const {chunkDim, blocks} = workerData;
  const {ChunkLoader} = chunks(chunkDim, blocks);

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
}