const assert = require('assert');
const ocean = require('./ocean');
const biomes = require('./biomes');
const math = require('./math');
const noise = require('../noise');
const multiblocks = require('./multiblocks');

function create(chunkDim, blocks) {

  const { blockIndex2D, blockIndex3D, centerChunkPos } = math(chunkDim);
  const { generateOceanMapForChunk } = ocean(chunkDim);
  const { biomeGenerator } = biomes(chunkDim);
  const { tree, house, MultiBlockRef } = multiblocks(chunkDim, blocks);

  const {
    'block.air': BLOCK_AIR,
    'block.grass': BLOCK_GRASS,
    'block.dirt': BLOCK_DIRT,
    'block.water': BLOCK_WATER,
    'block.wood': BLOCK_WOOD,
    'block.monkey': BLOCK_MONKEY
  } = blocks;

  const kChunkInitialized = 0;
  const kChunkOceanAndHeightMap = 1;
  const kChunkHeightMapSmoothed = 2;
  const kChunkMultiblocksGenerated = 3;
  const kChunkBlockFilled = 4;
  const kChunkMultiblocks = 5;
  const kChunkFinalized = 99;

  class Chunk {
    constructor(x, y, z) {
      this.x = x;
      this.y = y;
      this.z = z;

      this.data = new ArrayBuffer(4 * (chunkDim ** 3));
      this.blocks = new Int32Array(this.data);
      this.blocks.fill(BLOCK_AIR);

      this.state = kChunkInitialized;
      this.multiblockRefs = [];
    }
  }

  const SMOOTH_KERNEL = new Float32Array([
    0.05, 0.10, 0.25, 0.10, 0.05,
    0.10, 0.25, 0.50, 0.25, 0.10,
    0.25, 0.50, 1.00, 0.50, 0.25,
    0.10, 0.25, 0.50, 0.25, 0.10,
    0.05, 0.10, 0.25, 0.10, 0.05,
  ]);
  const SMOOTH_KERNEL_SIZE = 2;
  assert(SMOOTH_KERNEL.length === (2*SMOOTH_KERNEL_SIZE + 1) ** 2)

  const houseRef = new MultiBlockRef(house, -80, 15, 2);
  const treeRef = new MultiBlockRef(tree, -78, 15, 10);

  class ChunkLoader {
    constructor() {
      this._chunks = new Map();
      this._mapInfo = new Map();
    }

    /**
     * @param {number} x 
     * @param {number} y 
     * @param {number} z 
     * @return {Chunk}
     */
    getChunk(x, y, z) {
      return this._getChunkAdvanced(x, y, z, kChunkFinalized);
    }

    /**
     * @param {number} x 
     * @param {number} y 
     * @param {number} z 
     * @param {number} state 
     * @return {Chunk}
     */
    _getChunkAdvanced(x, y, z, state) {
      const chunkKey = genChunkKey(x, y, z);
      let chunk = this._chunks.get(chunkKey);

      if (!chunk) {
        chunk = new Chunk(x, y, z);
        this._chunks.set(chunkKey, chunk);
      }

      this._advanceChunkState(chunk, state);
      return chunk;
    }

    /**
     * @param {Chunk} chunk 
     */
    _advanceChunkState(chunk, state) {
      if (chunk.state === state) return;

      if (chunk.state === kChunkInitialized) {
        this._calculateOceanAndHeightMap(chunk.x, chunk.z);
        chunk.state = kChunkOceanAndHeightMap;
      }

      if (chunk.state === state) return;

      if (chunk.state === kChunkOceanAndHeightMap) {
        this._smoothHeightMap(chunk.x, chunk.z);
        chunk.state = kChunkHeightMapSmoothed;
      }

      if (chunk.state === state) return;

      if (chunk.state === kChunkHeightMapSmoothed) {
        this._generateMultiblocks(chunk);
        chunk.state = kChunkMultiblocksGenerated;
      }

      if (chunk.state === state) return;

      if (chunk.state === kChunkMultiblocksGenerated) {
        this._fillChunkWithBlocks(chunk);
        chunk.state = kChunkBlockFilled;
      }

      if (chunk.state === state) return;

      if (chunk.state === kChunkBlockFilled) {
        this._morphBlocks(chunk);
        chunk.state = kChunkMultiblocks;
      }

      if (chunk.state === state) return;

      if (chunk.state === kChunkMultiblocks) {
        this._applyMultiblocks(chunk);
        chunk.state = kChunkFinalized;
      }
    }

    /**
     * @param {number} chunkX 
     * @param {number} chunkZ 
     */
    _calculateOceanAndHeightMap(chunkX, chunkZ) {
      const mapInfoKey = genMapInfoKey(chunkX, chunkZ);
      const mapInfo = this._mapInfo.get(mapInfoKey);
      if (mapInfo) {
        return mapInfo;
      }

      const oceanMap = new Int8Array(chunkDim ** 2);
      generateOceanMapForChunk(oceanMap, chunkDim * chunkX, chunkDim * chunkZ);
      const { nearestPoints, nearestDistances } = biomeGenerator.generateChunk(chunkX, chunkZ);

      const xOffset = chunkDim * chunkX;
      const zOffset = chunkDim * chunkZ;

      const heightMap = new Float32Array(chunkDim ** 2);
      for (let x = 0; x < chunkDim; x++) {
        for (let z = 0; z < chunkDim; z++) {
          const blockIndex = blockIndex2D(x, z);
          if (oceanMap[blockIndex] !== 1) {
            const coordX = xOffset + x;
            const coordZ = zOffset + z;
            const height = nearestPoints[blockIndex].getHeight(coordX, coordZ);
            
            heightMap[blockIndex] = height;
          } else {
            heightMap[blockIndex] = 0;
          }
        }
      }

      const newMapInfo = {
        nearestPoints,
        nearestDistances,
        heightMap,
        oceanMap
      };

      this._mapInfo.set(mapInfoKey, newMapInfo);
      return newMapInfo;
    }

    /**
     * @param {number} chunkX 
     * @param {number} chunkZ 
     */
    _smoothHeightMap(chunkX, chunkZ) {
      const mapInfo = this._mapInfo.get(genMapInfoKey(chunkX, chunkZ));
      assert(mapInfo);
      
      const xOffset = chunkX * chunkDim;
      const zOffset = chunkZ * chunkDim;

      const smoothedHeightMap = new Float32Array(chunkDim ** 2);
      for (let x = 0; x < chunkDim; x++) {
        for (let z = 0; z < chunkDim; z++) {
          let sample = 0;
          let factorSum = 0;
          for (let kx = -SMOOTH_KERNEL_SIZE; kx <= SMOOTH_KERNEL_SIZE; kx++) {
            const ax = x + kx;
            const xCoord = xOffset + ax;

            for (let kz = -SMOOTH_KERNEL_SIZE; kz <= SMOOTH_KERNEL_SIZE; kz++) {
              const az = z + kz;
              const zCoord = zOffset + az;

              const smoothKernelIndex = (kx + SMOOTH_KERNEL_SIZE) + SMOOTH_KERNEL_SIZE * (kz + SMOOTH_KERNEL_SIZE);
              assert(smoothKernelIndex >= 0 && smoothKernelIndex < SMOOTH_KERNEL.length);

              const kernelFactor = SMOOTH_KERNEL[smoothKernelIndex];
              assert(isFinite(kernelFactor));
              
              factorSum += kernelFactor;
              
              let info, infoIndex;
              if ((ax >= 0 && ax < chunkDim) && (az >= 0 && az < chunkDim)) {
                info = mapInfo;
                infoIndex = blockIndex2D(ax, az);
                assert(infoIndex >= 0 && infoIndex < chunkDim ** 2);
              } else {
                const [chunkX, chunkZ] = centerChunkPos(xCoord, zCoord);
                info = this._calculateOceanAndHeightMap(chunkX, chunkZ);
                assert(info);

                const cx = xCoord - chunkX * chunkDim;
                const cz = zCoord - chunkZ * chunkDim;

                infoIndex = blockIndex2D(cx, cz);
                assert(infoIndex >= 0 && infoIndex < chunkDim ** 2);
              }

              const { heightMap } = info;
              assert(heightMap);

              const height = heightMap[infoIndex];
              assert(isFinite(height));

              sample += height * kernelFactor;
              assert(isFinite(sample));
            }
          }

          const heightMapIndex = blockIndex2D(x, z);
          const smoothSample = sample / factorSum
          assert(!isNaN(smoothSample));

          smoothedHeightMap[heightMapIndex] = smoothSample;
        }
      }

      mapInfo.smoothedHeightMap = smoothedHeightMap;
    }

    /**
     * @param {Chunk} chunk 
     */
    _generateMultiblocks(chunk) {

      if (treeRef.touchesChunk(chunk.x, chunk.y, chunk.z)) {
        chunk.multiblockRefs.push(treeRef);
      }

      if (houseRef.touchesChunk(chunk.x, chunk.y, chunk.z)) {
        chunk.multiblockRefs.push(houseRef);
      }
    }

    /**
     * @param {Chunk} chunk 
     */
    _fillChunkWithBlocks(chunk) {
      const xOffset = chunk.x * chunkDim;
      const yOffset = chunk.y * chunkDim;
      const zOffset = chunk.z * chunkDim;

      const mapInfoKey = genMapInfoKey(chunk.x, chunk.z);
      const mapInfo = this._mapInfo.get(mapInfoKey);
      assert(mapInfo);

      const { oceanMap, nearestDistances, smoothedHeightMap: heightMap } = mapInfo;
      const { blocks } = chunk;
      
      for (let x = 0; x < chunkDim; x++) {
        for (let z = 0; z < chunkDim; z++) {
          const isOcean = oceanMap[blockIndex2D(x, z)] == 1;
          if (isOcean) {
            for (let y = 0; y < chunkDim; y++) {
              const coordY = y + yOffset;
              if (coordY <= 0) {
                blocks[blockIndex3D(x, y, z)] = BLOCK_WATER;
              }
            }
          } else {
            const dd = blockIndex2D(x, z);
            const coordX = x + xOffset;
            const coordZ = z + zOffset;
    
            const distance = nearestDistances[dd];
            const height = heightMap[dd];
    
            for (let y = 0; y < Math.min(height - yOffset, chunkDim); y++) {
              const coordY = y + yOffset;
              const f = 80;
              const caveNoiseValue = (1 + noise.simplex3(coordX / f, coordY / f, coordZ / f)) / 2;
              if (caveNoiseValue * caveNoiseValue < 0.8) {
                const blockIndex = blockIndex3D(x, y, z);
                blocks[blockIndex] = BLOCK_GRASS;
              }
            }
    
            const pointY = height + 5;
            if (distance === 0 && yOffset <= pointY && pointY < yOffset + chunkDim) {
              const y = pointY - yOffset;
              blocks[blockIndex3D(x, y, z)] = BLOCK_WOOD;
            }
          }
        }
      }
    }

    /**
     * @param {Chunk} chunk 
     */
    _morphBlocks(chunk) {
      const { blocks } = chunk;
      for (let x = 0; x < chunkDim; x++) {
        for (let y = 0; y < chunkDim - 1; y++) {
          for (let z = 0; z < chunkDim; z++) {
            const blockIndex = blockIndex3D(x, y, z);
            const block = blocks[blockIndex];

            if (block === BLOCK_GRASS) {
              if (blocks[blockIndex3D(x, y + 1, z)] !== BLOCK_AIR) {
                blocks[blockIndex] = BLOCK_DIRT;
              }
            }
          }
        }
      }


      const topChunk = this._getChunkAdvanced(chunk.x, chunk.y + 1, chunk.z, kChunkBlockFilled);
      const { blocks: topChunkBlocks } = topChunk;
      for (let x = 0; x < chunkDim; x++) {
        for (let z = 0; z < chunkDim; z++) {
          const blockIndex = blockIndex3D(x, chunkDim - 1, z);
          const block = blocks[blockIndex];

          if (block === BLOCK_GRASS) {
            if (topChunkBlocks[blockIndex3D(x, 0, z)] !== BLOCK_AIR) {
              blocks[blockIndex] = BLOCK_DIRT;
            }
          }
        }
      }
    }

    /**
     * @param {Chunk} chunk 
     */
    _applyMultiblocks(chunk) {
      const coordX = chunk.x * chunkDim;
      const coordY = chunk.y * chunkDim;
      const coordZ = chunk.z * chunkDim;

      for (let i = 0; i < chunk.multiblockRefs.length; i++) {
        const multiblockRef = chunk.multiblockRefs[i];
        const { multiblock } = multiblockRef;

        const mbXOffset = multiblockRef.x - coordX;
        const mbYOffset = multiblockRef.y - coordY;
        const mbZOffset = multiblockRef.z - coordZ;

        const minX = Math.max(0,        mbXOffset);
        const maxX = Math.min(chunkDim, mbXOffset + multiblock.sx);
        const minY = Math.max(0,        mbYOffset);
        const maxY = Math.min(chunkDim, mbYOffset + multiblock.sy);
        const minZ = Math.max(0,        mbZOffset);
        const maxZ = Math.min(chunkDim, mbZOffset + multiblock.sz);

        for (let x = minX, mbX = minX - mbXOffset; x < maxX; x++, mbX++) {
          for (let y = minY, mbY = minY - mbYOffset; y < maxY; y++, mbY++) {
            for (let z = minZ, mbZ = minZ - mbZOffset; z < maxZ; z++, mbZ++) {
              const blockIndex = blockIndex3D(x, y, z);
              chunk.blocks[blockIndex] = multiblock.blockAt(mbX, mbY, mbZ);
            }
          }
        }
      }
    }
  }

  function genChunkKey(x, y, z) {
    return `${x}:${y}:${z}`;
  }

  function genMapInfoKey(x, z) {
    return `${x}:${z}`;
  }

  return {
    ChunkLoader
  };
};

module.exports = create;
