
module.exports = async function plugin(registry, opts) {
  const {modelLoader} = opts;
  if (!modelLoader) {
    throw new Error('modelLoader needed');
  }

  const [engine] = await Promise.all([
    registry.get('engine'),
    registry.get('render_pipeline'),  // TODO(daaitch): improper - needed for
                                      // gbuffer to set texture id
  ]);

  const models = await loadModels();

  return models;

  async function loadModels() {
    const loadedBlocks = [];
    const loadedSprites = [];

    for (const model of await modelLoader()) {
      if (model.id.startsWith('block.')) {
        loadedBlocks.push(model);
      }

      if (model.id.startsWith('sprite.')) {
        loadedSprites.push(model);
      }
    }

    const createdBlocksPromises = loadedBlocks.map(
        loadedBlock => {return engine.blockCreate(loadedBlock)
                            .then(handle => ({handle, id: loadedBlock.id}))});
    const createdSpritesPromises = loadedSprites.map(
        loadedSprite => {return engine.spriteCreate(loadedSprite)
                             .then(handle => ({handle, id: loadedSprite.id}))});

    const [createdBlocks, createdSprites] = await Promise.all([
      Promise.all(createdBlocksPromises), Promise.all(createdSpritesPromises)
    ]);

    const blocks = {};
    for (const createdBlock of createdBlocks) {
      blocks[createdBlock.id] = createdBlock.handle;
    }

    const sprites = {};
    for (const createdSprite of createdSprites) {
      sprites[createdSprite.id] = createdSprite.handle;
    }

    await engine.blockBake();
    return {blocks, sprites};
  }
}
