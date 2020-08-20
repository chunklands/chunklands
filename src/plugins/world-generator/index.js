const debug = require('debug')('world_generator');
const ChunkManager = require('./ChunkManager');
const createBatchCall = require('../../lib/batchCall');

module.exports = async function worldGenerator(registry, opts) {
  const [ api, blocks ] = await Promise.all([
    registry.get('api'),
    registry.get('blocks')
  ]);

  const chunkManager = new ChunkManager(api, blocks);
  api.cameraGetPosition().then(cameraPos => chunkManager.updatePosition(cameraPos));

  const cleanup = createBatchCall()
    .add(api.cameraOn('positionchange', event => {
      chunkManager.updatePosition(event);
    }))
    .add(() => chunkManager.terminate())
    .call;

  return {
    onTerminate: cleanup
  };
};
