import { createSimplexNoise } from './algorithm';
import createMath from './math';

const oceanNoise = createSimplexNoise({
  f0: 128,
  octaves: 8,
  persistence: 0.9,
  lacunarity: 4.178,
});

export default function createOcean(chunkDim: number) {
  const { blockIndex2D } = createMath(chunkDim);

  function generateOceanMapForChunk(
    oceanMap: Int8Array,
    xOffset: number,
    zOffset: number
  ) {
    for (let px = 0; px < chunkDim; px++) {
      for (let pz = 0; pz < chunkDim; pz++) {
        const sample = oceanNoise(xOffset + px, zOffset + pz);
        oceanMap[blockIndex2D(px, pz)] = sample > 0.5 ? 1 : 0;
      }
    }
  }

  return { generateOceanMapForChunk };
}
