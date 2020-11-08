import createBiomes from "./biomes";
import createMath from "./math";
import createMultiblocks from "./multiblocks";
import createOcean from "./ocean";
import assert from 'assert'
import { simplex3 } from "../../../lib/noise";
import { IChunk, IChunkLoader } from "./ChunkPrioLoader";

export default function createChunks(chunkDim: number, blocks: {[id: string]: bigint}) {
  const {blockIndex2D, blockIndex3D, centerChunkPos} = createMath(chunkDim);
  const {generateOceanMapForChunk} = createOcean(chunkDim);
  const {biomeGenerator} = createBiomes(chunkDim);
  const {tree, house, MultiBlockRef} = createMultiblocks(chunkDim, blocks);

  const {
    'block.air': BLOCK_AIR,
    'block.grass': BLOCK_GRASS,
    'block.dirt': BLOCK_DIRT,
    'block.water': BLOCK_WATER,
    'block.wood': BLOCK_WOOD,
    'block.monkey': BLOCK_MONKEY,
    'block.pickaxe': BLOCK_PICKAXE,
  } = blocks;

  enum ChunkState {
    kInitialized,
    kOceanAndHeightMap,
    kHeightMapSmoothed,
    kMultiblocksGenerated,
    kBlockFilled,
    kMultiblocks,
    kFinalized,
  }

  class Chunk implements IChunk {
    public data = new ArrayBuffer(BigUint64Array.BYTES_PER_ELEMENT * (chunkDim ** 3));
    public blocks: BigUint64Array
    public state: ChunkState = ChunkState.kInitialized

    public multiblockRefs = []

    constructor(public x: number, public y: number, public z: number) {          
      this.blocks = new BigUint64Array(this.data);
      this.blocks.fill(BLOCK_AIR);
    }
  }

  const SMOOTH_KERNEL = new Float32Array([
    0.05, 0.10, 0.25, 0.10, 0.05, 0.10, 0.25, 0.50, 0.25,
    0.10, 0.25, 0.50, 1.00, 0.50, 0.25, 0.10, 0.25, 0.50,
    0.25, 0.10, 0.05, 0.10, 0.25, 0.10, 0.05,
  ]);
  const SMOOTH_KERNEL_SIZE = 2;
  assert(SMOOTH_KERNEL.length === (2 * SMOOTH_KERNEL_SIZE + 1) ** 2)

  const houseRef = new MultiBlockRef(house, -80, 15, 2);
  const treeRef = new MultiBlockRef(tree, -78, 15, 10);

  class ChunkLoader implements IChunkLoader {
    private chunks = new Map();
    private mapInfo = new Map();

    getChunk(x: number, y: number, z: number) {
      return this.getChunkAdvanced(x, y, z, ChunkState.kFinalized);
    }

    private getChunkAdvanced(x: number, y: number, z: number, state: ChunkState) {
      const chunkKey = genChunkKey(x, y, z);
      let chunk = this.chunks.get(chunkKey);

      if (!chunk) {
        chunk = new Chunk(x, y, z);
        this.chunks.set(chunkKey, chunk);
      }

      this.advanceChunkState(chunk, state);
      return chunk;
    }

    private advanceChunkState(chunk: Chunk, state: ChunkState) {
      if (chunk.state === state) return;

      if (chunk.state === ChunkState.kInitialized) {
        this.calculateOceanAndHeightMap(chunk.x, chunk.z);
        chunk.state = ChunkState.kOceanAndHeightMap;
      }

      if (chunk.state === state) return;

      if (chunk.state === ChunkState.kOceanAndHeightMap) {
        this.smoothHeightMap(chunk.x, chunk.z);
        chunk.state = ChunkState.kHeightMapSmoothed;
      }

      if (chunk.state === state) return;

      if (chunk.state === ChunkState.kHeightMapSmoothed) {
        this.generateMultiblocks(chunk);
        chunk.state = ChunkState.kMultiblocksGenerated;
      }

      if (chunk.state === state) return;

      if (chunk.state === ChunkState.kMultiblocksGenerated) {
        this.fillChunkWithBlocks(chunk);
        chunk.state = ChunkState.kBlockFilled;
      }

      if (chunk.state === state) return;

      if (chunk.state === ChunkState.kBlockFilled) {
        this.morphBlocks(chunk);
        chunk.state = ChunkState.kMultiblocks;
      }

      if (chunk.state === state) return;

      if (chunk.state === ChunkState.kMultiblocks) {
        this.applyMultiblocks(chunk);
        chunk.state = ChunkState.kFinalized;
      }
    }

    private calculateOceanAndHeightMap(chunkX: number, chunkZ: number) {
      const mapInfoKey = genMapInfoKey(chunkX, chunkZ);
      const mapInfo = this.mapInfo.get(mapInfoKey);
      if (mapInfo) {
        return mapInfo;
      }

      const oceanMap = new Int8Array(chunkDim ** 2);
      generateOceanMapForChunk(oceanMap, chunkDim * chunkX, chunkDim * chunkZ);
      const {nearestPoints, nearestDistances} =
          biomeGenerator.generateChunk(chunkX, chunkZ);

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

      const newMapInfo = {nearestPoints, nearestDistances, heightMap, oceanMap};

      this.mapInfo.set(mapInfoKey, newMapInfo);
      return newMapInfo;
    }

    private smoothHeightMap(chunkX: number, chunkZ: number) {
      const mapInfo = this.mapInfo.get(genMapInfoKey(chunkX, chunkZ));
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

              const smoothKernelIndex = (kx + SMOOTH_KERNEL_SIZE) +
                  SMOOTH_KERNEL_SIZE * (kz + SMOOTH_KERNEL_SIZE);
              assert(
                  smoothKernelIndex >= 0 &&
                  smoothKernelIndex < SMOOTH_KERNEL.length);

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
                info = this.calculateOceanAndHeightMap(chunkX, chunkZ);
                assert(info);

                const cx = xCoord - chunkX * chunkDim;
                const cz = zCoord - chunkZ * chunkDim;

                infoIndex = blockIndex2D(cx, cz);
                assert(infoIndex >= 0 && infoIndex < chunkDim ** 2);
              }

              const {heightMap} = info;
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

    private generateMultiblocks(chunk: Chunk) {
      if (treeRef.touchesChunk(chunk.x, chunk.y, chunk.z)) {
        chunk.multiblockRefs.push(treeRef);
      }

      if (houseRef.touchesChunk(chunk.x, chunk.y, chunk.z)) {
        chunk.multiblockRefs.push(houseRef);
      }
    }

    private fillChunkWithBlocks(chunk: Chunk) {
      const xOffset = chunk.x * chunkDim;
      const yOffset = chunk.y * chunkDim;
      const zOffset = chunk.z * chunkDim;

      const mapInfoKey = genMapInfoKey(chunk.x, chunk.z);
      const mapInfo = this.mapInfo.get(mapInfoKey);
      assert(mapInfo);

      const {oceanMap, nearestDistances, smoothedHeightMap: heightMap} =
          mapInfo;
      const {blocks} = chunk;

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
              const caveNoiseValue =
                  (1 + simplex3(coordX / f, coordY / f, coordZ / f)) / 2;
              if (caveNoiseValue * caveNoiseValue < 0.8) {
                const blockIndex = blockIndex3D(x, y, z);
                // if (x === 16 && y === 16 && z === 16) {
                //   blocks[blockIndex] = BLOCK_PICKAXE;
                // } else {
                blocks[blockIndex] = BLOCK_GRASS;
                // }
              }
            }
          }
        }
      }
    }

    private morphBlocks(chunk: Chunk) {
      const {blocks} = chunk;
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


      const topChunk = this.getChunkAdvanced(
          chunk.x, chunk.y + 1, chunk.z, ChunkState.kBlockFilled);
      const {blocks: topChunkBlocks} = topChunk;
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

    private applyMultiblocks(chunk: Chunk) {
      const coordX = chunk.x * chunkDim;
      const coordY = chunk.y * chunkDim;
      const coordZ = chunk.z * chunkDim;

      for (let i = 0; i < chunk.multiblockRefs.length; i++) {
        const multiblockRef = chunk.multiblockRefs[i];
        const {multiblock} = multiblockRef;

        const mbXOffset = multiblockRef.x - coordX;
        const mbYOffset = multiblockRef.y - coordY;
        const mbZOffset = multiblockRef.z - coordZ;

        const minX = Math.max(0, mbXOffset);
        const maxX = Math.min(chunkDim, mbXOffset + multiblock.sx);
        const minY = Math.max(0, mbYOffset);
        const maxY = Math.min(chunkDim, mbYOffset + multiblock.sy);
        const minZ = Math.max(0, mbZOffset);
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

  function genChunkKey(x: number, y: number, z: number) {
    return `${x}:${y}:${z}`;
  }

  function genMapInfoKey(x: number, z: number) {
    return `${x}:${z}`;
  }

  return {ChunkLoader};
};
