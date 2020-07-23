const game = require('./_');
const gl = require('../gl');
const bakeModels = require('./bakeModels');

class ModelLoader {
  constructor() {
    this.blockRegistrar = new game.BlockRegistrar();
    this.spriteRegistrar = new game.SpriteRegistrar();

    this.blockIds = {};
  }

  /**
   * Loads and bake models.
   * 
   * @param {() => Promise<any>)} loader Loader function
   * @param {string} texturePath Path where to write cached texture
   */
  async load(loader, texturePath) {
    const models = await loader();
    const bakedModels = await bakeModels(models);

    for (const bakedModel of bakedModels.models) {
      if (bakedModel.id.startsWith('block.')) {
        const blockId = this.blockRegistrar.addBlock(bakedModel);
        this.blockIds[bakedModel.id] = blockId;
      } else if (bakedModel.id.startsWith('sprite.')) {
        this.spriteRegistrar.addSprite(bakedModel);
      }
    }

    await bakedModels.texture.writeAsync(texturePath);
    this.texture = new gl.Texture2();
    this.texture.load(texturePath);

    // NO:
    // this._blockRegistrar.loadTexture(texturePath);
  }
}

module.exports = ModelLoader;