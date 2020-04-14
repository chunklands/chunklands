const noise = require('../../../noise');

module.exports = {
  isGround(x, y, z) {
    const v = noise.perlin3(x / 40, y / 20, z / 40);
    console.log({v});
    return v * 200 > y;
  }
};
