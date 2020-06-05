const { parentPort, workerData, isMainThread } = require('worker_threads');
const { BiomeGenerator } = require('../biomes');
const algorithm = require('./algorithm');
const noise = require('../../noise');
const biomes = require('./Biome');

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
const {
  'block.air': BLOCK_AIR,
  'block.grass': BLOCK_GRASS,
  'block.water': BLOCK_WATER,
  'block.wood': BLOCK_WOOD,
  'block.monkey': BLOCK_MONKEY
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

let spotCount = 0;
let generatedChunks = 0;
setInterval(() => {
  console.log(`spots per chunk = ${spotCount / generatedChunks}`)
}, 500);

const biomeNoise = algorithm.createSimplexNoise({f0: 32, octaves: 3, persistence: 0.5, lacunarity: 3.1});
const spotsNoise = algorithm.createSimplexNoise({f0: 8, octaves: 8, persistence: 0.56, lacunarity: 1.34});
const biomeGenerator = new BiomeGenerator(chunkDim, (chunkX, chunkZ) => {
  const spotNoiseValue = spotsNoise(chunkX, chunkZ);
  const hasSpot = spotNoiseValue > 0.5;

  generatedChunks++;
  if (!hasSpot) {
    return [];
  }
  spotCount++;

  const x = (spotNoiseValue * 731323) & (chunkDim - 1);
  const z = (spotNoiseValue * 138234) & (chunkDim - 1);
  const coordX = chunkX * chunkDim + x;
  const coordZ = chunkZ * chunkDim + z;

  const biomeNoiseValue = biomeNoise(coordX, coordZ);
  const biome = (biomeNoiseValue * 3)|0;
  switch (biome) {
    case 0: return [new biomes.ExtremeMountainBiome(coordX, coordZ)];
    case 1: return [new biomes.MountainBiome(coordX, coordZ)];
    case 2: return [new biomes.FlatBiome(coordX, coordZ)];
  }

  return [];
});

/**
 * @param {Int32Array} buf 
 * @param {number} chunkX 
 * @param {number} chunkY 
 * @param {number} chunkZ 
 */
function generateChunk(buf, chunkX, chunkY, chunkZ) {
  buf.fill(BLOCK_AIR);

  const xOffset = chunkX * chunkDim;
  const yOffset = chunkY * chunkDim;
  const zOffset = chunkZ * chunkDim;

  const { nearestPoints, nearestDistances } = biomeGenerator.generateChunk(chunkX, chunkZ);

  const oceanMap = new Int8Array(chunkDim ** 3)
  generateOceanMapForChunk(oceanMap, xOffset, zOffset);

  for (let x = 0; x < chunkDim; x++) {
    for (let z = 0; z < chunkDim; z++) {
      const isOcean = oceanMap[blockIndex2D(x, z)] == 1;
      if (isOcean) {
        for (let y = 0; y < chunkDim; y++) {
          const coordY = y + yOffset;
          if (coordY <= 0) {
            buf[blockIndex3D(x, y, z)] = BLOCK_WATER
          }
        }
      } else {
        const dd = blockIndex2D(x, z);
        const coordX = x + xOffset;
        const coordZ = z + zOffset;

        // TODO(daaitch): more nearest Points for interpolation
        const biome = nearestPoints[dd];
        const distance = nearestDistances[dd];
        const height = biome.getHeight(coordX, coordZ)|0;

        for (let y = 0; y < Math.min(height - yOffset, chunkDim); y++) {
          const coordY = y + yOffset;
          const f = 23.1;
          const caveNoiseValue = (1 + noise.simplex3(coordX / f, coordY / f, coordZ / f)) / 2;
          if (caveNoiseValue > 0.2) {
            const blockIndex = blockIndex3D(x, y, z);
            buf[blockIndex] = BLOCK_GRASS;
          }
        }

        const pointY = height + 5;
        if (distance === 0 && yOffset <= pointY && pointY < yOffset + chunkDim) {
          const y = pointY - yOffset;
          buf[blockIndex3D(x, y, z)] = BLOCK_WOOD;
        }
      }
    }
  }
}

const oceanNoise = algorithm.createSimplexNoise({f0: 128, octaves: 8, persistence: 0.9, lacunarity: 4.178});

let sampleMax = 0, sampleMin = 1;

setInterval(() => {
  console.log({sampleMin, sampleMax})
}, 500)

function generateOceanMapForChunk(oceanMap, xOffset, zOffset) {
  for (let px = 0; px < chunkDim; px++) {
    for (let pz = 0; pz < chunkDim; pz++) {
      const sample = oceanNoise(xOffset + px, zOffset + pz);
      if (sample > sampleMax) {
        sampleMax = sample;
      }

      if (sample < sampleMin) {
        sampleMin = sample;
      }
      oceanMap[blockIndex2D(px, pz)] = sample > 0.5 ? 1 : 0;
    }
  }
}

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