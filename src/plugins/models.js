
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
    for (const model of await modelLoader()) {
      if (model.id.startsWith('block.')) {
        engine.blockCreate(model)
      }

      if (model.id.startsWith('sprite.')) {
        engine.spriteCreate(model)
      }
    }

    return await engine.blockBake();
  }
}
