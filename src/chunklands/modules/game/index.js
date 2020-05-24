const game = require('./game_module');
const engine = require('../engine');
const assert = require('assert');
const {promisify} = require('util');
const imageSize = promisify(require('image-size'));
const Jimp = require('jimp');
const { BiomeGenerator } = require('./biomes');

const files = require('../../../files');

class BlockRegistrar extends game.BlockRegistrarBase {

  constructor() {
    super();

    this._blocks = [];
  }

  addBlock(block) {
    this._blocks.push(block);
  }

  async bake() {
    const baker = new engine.TextureBaker();

    const blocksAndAreas = await Promise.all(
      this._blocks.map(async block => {
        if (!block.texture) {
          return {block};
        }

        const {width, height} = await imageSize(block.texture);

        const area = baker.addArea(width, height);
        return {area, block};
      })
    );

    const n = getNextPOT(baker.maxDim);
    const textureDim = 1 << n;

    const image = new Jimp(textureDim, textureDim);
    await Promise.all(blocksAndAreas.map(async ({block, area}) => {
      if (area) {
        assert.ok(block.texture)

        const texture = await Jimp.create(block.texture);
        assert.strictEqual(texture.getWidth(), area.w);
        assert.strictEqual(texture.getHeight(), area.h);

        image.blit(texture, area.x, area.y);

        const uvCorrectedBlock = {...block};
        const vertexData = uvCorrectedBlock.vertexData = {...uvCorrectedBlock.vertexData};
        for (const face of Object.keys(vertexData)) {
          const faceVertexData = vertexData[face] = [...vertexData[face]];
          for (let v = 0; v < faceVertexData.length / 8; v++) {
            const uIndex = v * 8 + 6;
            const vIndex = uIndex + 1;

            faceVertexData[uIndex] = (area.x + (faceVertexData[uIndex] * area.w)) / textureDim;
            faceVertexData[vIndex] = (area.y + (faceVertexData[vIndex] * area.h)) / textureDim;
          }
        }

        super.addBlock(uvCorrectedBlock);
      } else {
        super.addBlock(block);
      }
    }));

    const textureCacheFilePath = files.cacheFile('texture.png');
    await image.writeAsync(textureCacheFilePath);

    super.loadTexture(textureCacheFilePath);
  }
};

function getNextPOT(num) {
  let n;
  for (n = 0; (1 << n) < num; n++) {}

  return n;
}

module.exports = {
  BlockRegistrar,
  ...game,
  BiomeGenerator
};
