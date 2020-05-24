
const noise = require('../../noise');

function createPerlinNoise({f0 = 128, lacunarity, persistence, octaves, amplitude}) {
  return (x, y) => {
    let sample = 0;

    let f = f0; // freq
    let A = 1; // amplitude
    let Asum = 0;

    for (let i = 0; i < octaves; i++) {
      sample += A * noise.perlin2(x / f, y / f);

      f *= lacunarity;
      Asum += A;
      A *= persistence;
    }

    return sample / Asum * amplitude;
  };
}

/**
 * @param {[number, any][]} ranges must be sorted
 */
function createSegregator(ranges) {
  if (!ranges || ranges.length === 0) {
    throw new Error('need at least one range');
  }

  return sample => {
    for (let i = 0; i < ranges.length - 1; i++) {
      if (sample >= ranges[i][0]) {
        return ranges[i][1];
      }
    }

    return ranges[ranges.length - 1][1];
  };
}

module.exports = {
  createPerlinNoise,
  createSegregator
};
