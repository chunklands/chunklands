const algorithm = require('./algorithm')

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
      f0: 32,
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
      f0: 64,
      lacunarity: 1.2,
      persistence: 0.
    })
  }

  getHeight(x, z) {
    return 3 * this._height(x, z);
  }
}

module.exports = {
  MountainBiome,
  FlatBiome,
  ExtremeMountainBiome
};
