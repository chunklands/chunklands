const game = require('./game_module');
const engine = require('../engine');
const assert = require('assert');
const { EventEmitter } = require('events')
const {promisify} = require('util');
const imageSize = promisify(require('image-size'));
const Jimp = require('jimp');
const { Scene } = require('./game_module');

const files = require('../../../files');

class ResourceRegistrar {

  /**
   * @param {import('./game_module').BlockRegistrar} blockRegistrar 
   */
  constructor(blockRegistrar) {
    this._unbakedBlocks = [];
    this._blocks = {};
    this._blockRegistrar = blockRegistrar;
  }

  addBlock(block) {
    this._unbakedBlocks.push(block);
  }

  async bake() {
    const baker = new engine.TextureBaker();

    const blocksAndAreas = await Promise.all(
      this._unbakedBlocks.map(async block => {
        if (!block.texture) {
          return {area: null, block};
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
      let blockOffset;
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

        blockOffset = this._blockRegistrar.addBlock(uvCorrectedBlock);
      } else {
        blockOffset = this._blockRegistrar.addBlock(block);
      }

      this._blocks[block.id] = blockOffset;
    }));

    const textureCacheFilePath = files.cacheFile('texture.png');
    await image.writeAsync(textureCacheFilePath);

    this._blockRegistrar.loadTexture(textureCacheFilePath);
  }

  getBlockIds() {
    return this._blocks;
  }
};

function getNextPOT(num) {
  let n;
  for (n = 0; (1 << n) < num; n++) {}

  return n;
}

function createScene() {
  const scene = new Scene();
  scene.events = new EventEmitter();

  return scene;
}

module.exports = {
  ResourceRegistrar,
  ...game,
  createScene
};
