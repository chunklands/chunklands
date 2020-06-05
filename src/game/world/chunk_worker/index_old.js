const { BiomeGenerator } = require('../../../chunklands/modules/game');
const assert = require('assert');
const noise = require('../../noise');
const algorithm = require('./algorithm');
noise.seed(94832281);

const { parentPort, workerData } = require('worker_threads');

const {
  'block.stone': BLOCK_STONE,
  'block.grass': BLOCK_GRASS,
  'block.dirt': BLOCK_DIRT,
  'block.air': BLOCK_AIR,
  'block.water': BLOCK_WATER,
  'block.sand': BLOCK_SAND,
  'block.monkey': BLOCK_MONKEY,
  'block.wood': BLOCK_WOOD,
} = workerData;

if (BLOCK_STONE === undefined
  || BLOCK_GRASS === undefined
  || BLOCK_DIRT === undefined
  || BLOCK_AIR === undefined
  || BLOCK_WATER === undefined
  || BLOCK_MONKEY === undefined
  || BLOCK_WOOD === undefined
) {
  throw new TypeError(`bad workerData: ${JSON.stringify(workerData)}`);
}

parentPort.on('message', ({x, y, z, chunkDim, sendPort}) => {
  const chunk = generateChunk(x, y, z, chunkDim);
  
  sendPort.postMessage(chunk);
  sendPort.close();
});


const crazyMountainHFn = algorithm.createPerlinNoise({f0: 50, lacunarity: 8.733, persistence: 0.7, octaves: 20, amplitude: 100});
const mountainHFn = algorithm.createPerlinNoise({f0: 100, lacunarity: 2.5, persistence: 0.5, octaves: 5, amplitude: 50});
const flatHFn = algorithm.createPerlinNoise({f0: 1024, lacunarity: 1.3, persistence: 0.3, octaves: 3, amplitude: 20});


const landSeg = algorithm.createSegregator([
  [30, BLOCK_STONE],
  [10, BLOCK_GRASS],
  [1, BLOCK_SAND],
  [0, BLOCK_WATER]
]);

const mountainsSeg = algorithm.createSegregator([
  [1, BLOCK_STONE],
  [0, BLOCK_WATER]
]);

function createBlockFn({heightMapFn, segregator}) {
  return (x, y, z) => {
    const height = Math.max(0, heightMapFn(x, z));
    if (y <= height) {
      return segregator(y);
    }

    return BLOCK_AIR;
  };
}

const biomeGenerator = new BiomeGenerator(16, (cx, cz) => {
  const prn = noise.prng2(cx, cz, 0.415, 0.0948);
  assert(prn >= 0 && prn <= 1);

  const spotCount = (2 * prn)|0;

  const spots = []
  for (let i = 0; i < spotCount; i++) {
    const r = 16 * 16 * noise.prng3(cx, cz, i, 0.319, 0.0041);
    assert (r >= 0 && r < 16*16);

    let blockFn;
    switch ((3*noise.prng3(cx, cz, i, 0.3881, 0.7277))|0) {
      case 0: {
        blockFn = createBlockFn({heightMapFn: crazyMountainHFn, segregator: mountainsSeg});
        break;
      }
      case 1: {
        blockFn = createBlockFn({heightMapFn: mountainHFn, segregator: mountainsSeg});
        break;
      }
      case 2: {
        blockFn = createBlockFn({heightMapFn: flatHFn, segregator: landSeg});
        break;
      }
    }

    spots.push({x: cx * 16 + r%16, z: cz * 16 + (r/16)|0, blockFn})
  }

  return spots;
});

function generateChunk(x, y, z, chunkDim) {
  // TODO(daaitch): use "movable" buffers e.g. ArrayBuffer
  const chunk = new Array(chunkDim * chunkDim * chunkDim);

  const chunkOffsetX = x * chunkDim;
  const chunkOffsetY = y * chunkDim;
  const chunkOffsetZ = z * chunkDim;

  const chunkInfo = biomeGenerator.generateChunk(x, z);

  const heightMap = new Array(chunkDim * chunkDim).fill(-1);

  let i = 0;
  for (let lz = 0; lz < chunkDim; lz++) {
    for (let ly = 0; ly < chunkDim; ly++) {
      for (let lx = 0; lx < chunkDim; lx++) {
        const ii = lz * chunkDim + lx;
        const blockInfo = chunkInfo.nearestPoints[ii];
        const distanceInfo = chunkInfo.nearestDistances[ii];

        assert(blockInfo    !== undefined);
        assert(distanceInfo !== undefined);

        const coordX = chunkOffsetX + lx;
        const coordY = chunkOffsetY + ly;
        const coordZ = chunkOffsetZ + lz;

        // TODO(daaitch): optimize loops
        if (coordX === 0 && coordY === 10 && coordZ === -4) {
          chunk[i] = BLOCK_MONKEY;
        } else {
          const block = blockInfo.blockFn(coordX, coordY, coordZ);
          chunk[i] = block;

          if (block !== BLOCK_AIR) {
            const heightMapIndex = lz * chunkDim + lx;
            heightMap[heightMapIndex] = Math.max(heightMap[heightMapIndex], ly);
          }
        }

        i++;
      }
    }
  }

  for (let lz = 0; lz < chunkDim; lz++) {
    for (let lx = 0; lx < chunkDim; lx++) {
      const height = heightMap[lz * chunkDim + lx];
      if (height === -1 || height >= chunkDim) {
        continue;
      }

      const block = chunk[lx + chunkDim * (height + chunkDim * lz)];

      if (block === BLOCK_WATER || block === BLOCK_SAND) {
        continue;
      }

      const prn = noise.prng2(chunkOffsetX + lx, chunkOffsetZ + lz, 0.8413, 3298.48);
      if (prn > 0.2) {
        continue;
      }

      for (let h = height; h < Math.min(chunkDim, height + 5); h++) {
        chunk[lx + chunkDim * (h + chunkDim * lz)] = BLOCK_WOOD;
      }
    }
  }

  return chunk;
}
