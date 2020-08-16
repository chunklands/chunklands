
const modelLoader = require('../assets/models');

module.exports = async function blockLoader(api) {
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