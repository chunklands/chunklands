
const noise = require('../../noise');
noise.seed(9489032840928);

function createSimplexNoise({f0 = 128, lacunarity, persistence, octaves = 1}) {
  return (x, y) => {
    let sample = 0;

    let f = f0; // freq
    let A = 1; // amplitude
    let Asum = 0;

    for (let i = 0; i < octaves; i++) {
      sample += A * noise.simplex2(x / f, y / f);
      Asum += A;

      f *= lacunarity;
      A *= persistence;
    }

    // normalize
    return (1 + sample / Asum) / 2;
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
  createSimplexNoise,
  createSegregator
};
