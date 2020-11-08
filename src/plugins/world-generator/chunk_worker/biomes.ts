import { BiomeGenerator, PointXZ } from '../biomes';
import { createSimplexNoise, SimpleNoise } from './algorithm';

class MountainBiome implements PointXZ {
  private height: SimpleNoise;

  constructor(public x: number, public z: number) {
    this.height = createSimplexNoise({
      f0: 256,
      lacunarity: 3.6,
      persistence: 0.5,
      octaves: 7,
    });
  }

  getHeight(x: number, z: number) {
    return 50 * this.height(x, z);
  }
}

class ExtremeMountainBiome implements PointXZ {
  private height: SimpleNoise;
  constructor(public x: number, public z: number) {
    this.height = createSimplexNoise({
      f0: 11,
      lacunarity: 8.41,
      persistence: 0.8,
      octaves: 7,
    });
  }

  getHeight(x: number, z: number) {
    return 100 * this.height(x, z);
  }
}

class FlatBiome implements PointXZ {
  private height: SimpleNoise;
  constructor(public x: number, public z: number) {
    this.height = createSimplexNoise({ f0: 64, lacunarity: 1, persistence: 1 });
  }

  getHeight(x: number, z: number) {
    return 3 * this.height(x, z);
  }
}

export default function createBiomes(chunkDim: number) {
  // let spotCount = 0;
  // let generatedChunks = 0;

  // setInterval(() => {
  //   console.log(`spots per chunk = ${spotCount / generatedChunks}`)
  // }, 500);

  const biomeNoise = createSimplexNoise({
    f0: 32,
    octaves: 3,
    persistence: 0.5,
    lacunarity: 3.1,
  });
  const spotsNoise = createSimplexNoise({
    f0: 8,
    octaves: 8,
    persistence: 0.56,
    lacunarity: 1.34,
  });
  const biomeGenerator = new BiomeGenerator(chunkDim, (chunkX, chunkZ) => {
    const spotNoiseValue = spotsNoise(chunkX, chunkZ);
    const hasSpot = spotNoiseValue > 0.5;

    // generatedChunks++;
    if (!hasSpot) {
      return [];
    }
    // spotCount++;

    const x = (spotNoiseValue * 731323) & (chunkDim - 1);
    const z = (spotNoiseValue * 138234) & (chunkDim - 1);
    const coordX = chunkX * chunkDim + x;
    const coordZ = chunkZ * chunkDim + z;

    const biomeNoiseValue = biomeNoise(coordX, coordZ);
    const biome = (biomeNoiseValue * 3) | 0;
    switch (biome) {
      case 0:
        return [new ExtremeMountainBiome(coordX, coordZ)];
      case 1:
        return [new MountainBiome(coordX, coordZ)];
      case 2:
        return [new FlatBiome(coordX, coordZ)];
    }

    return [];
  });

  return { biomeGenerator };
}
