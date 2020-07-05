const algorithm = require('./algorithm');
const { BiomeGenerator } = require('../biomes');
class MountainBiome {
  constructor(x, z) {
    this.x = x;
    this.z = z;

    this._height = algorithm.createSimplexNoise({
      f0: 256,
      lacunarity: 3.6,
      persistence: 0.5,
      octaves: 7
    });
  }

  getHeight(x, z) {
    return 50 * this._height(x, z);
  }
}

class ExtremeMountainBiome {
  constructor(x, z) {
    this.x = x;
    this.z = z;

    this._height = algorithm.createSimplexNoise({
      f0: 11,
      lacunarity: 8.41,
      persistence: 0.8,
      octaves: 7
    });
  }

  getHeight(x, z) {
    return 100 * this._height(x, z);
  }
}

class FlatBiome {
  constructor(x, z) {
    this.x = x;
    this.z = z;

    this._height = algorithm.createSimplexNoise({
      f0: 64
    });
  }

  getHeight(x, z) {
    return 3 * this._height(x, z);
  }
}

/**
 * @param {number} chunkDim 
 */
function create(chunkDim) {
  let spotCount = 0;
  let generatedChunks = 0;

  // setInterval(() => {
  //   console.log(`spots per chunk = ${spotCount / generatedChunks}`)
  // }, 500);

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
      case 0: return [new ExtremeMountainBiome(coordX, coordZ)];
      case 1: return [new MountainBiome(coordX, coordZ)];
      case 2: return [new FlatBiome(coordX, coordZ)];
    }

    return [];
  });

  return {
    biomeGenerator
  };
}

module.exports = create;
