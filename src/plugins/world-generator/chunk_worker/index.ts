import { parentPort, workerData, isMainThread } from 'worker_threads';
import chunks from './chunks';
import ChunkPrioLoader from './ChunkPrioLoader';

export const filename = __filename;

if (!isMainThread) {
  // WorldGenerator:
  // 1. ocean / land via simplex noise
  // 2. Biomes + HeightMap via voronoi cells
  // 3. biome dependant heightMap smoothing
  // 4. caves
  // 5. multi-blocks

  if (!parentPort) {
    throw new Error('expect parent port');
  }

  const { chunkDim, blocks } = workerData;
  const { ChunkLoader } = chunks(chunkDim, blocks);

  const prioLoader = new ChunkPrioLoader(new ChunkLoader());

  parentPort.on('message', ({ type, payload }) => {
    if (type === 'updateposition') {
      prioLoader.updatePositionAndSort(payload);
    } else if (type === 'requestchunk') {
      prioLoader.addChunk(payload.pos, payload.sendPort);
    }
  });
}
