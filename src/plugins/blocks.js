
module.exports = async function plugin(registry, opts) {
  const { modelLoader } = opts;
  if (!modelLoader) {
    throw new Error('modelLoader needed');
  }

  const [ api ] = await Promise.all([
    registry.get('api'),
    registry.get('render_pipeline'), // TODO(daaitch): improper - needed for gbuffer to set texture id
  ]);

  const blocks = await loadBlocks();

  return blocks;

  async function loadBlocks() {
    const loadedBlocks = [];
    for (const model of await modelLoader()) {
      if (model.id.startsWith('block.')) {
        loadedBlocks.push(model);
      }
    }
    
    const createdBlocksPromises = loadedBlocks.map(loadedBlock => {
      return api.blockCreate(loadedBlock)
        .then(handle => ({handle, id: loadedBlock.id}))
    })
    const createdBlocks = await Promise.all(createdBlocksPromises)
  
    const blocks = {};
    for (const createdBlock of createdBlocks) {
      blocks[createdBlock.id] = createdBlock.handle;
    }
  
    await api.blockBake();
    return blocks;
  }
}

