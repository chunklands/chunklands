const plugin = require('../plugin');
const ChunkManager = require('./ChunkManager');
const createBatchCall = require('../../lib/batchCall');

module.exports = async function worldGenerator(registry) {
  const [ api, blocks ] = await Promise.all([
    registry.get('api'),
    registry.get('blocks')
  ]);

  const chunkManager = new ChunkManager(api, blocks);
  api.cameraGetPosition().then(cameraPos => chunkManager.updatePosition(cameraPos));

  const cleanup = createBatchCall();
  cleanup
    .add(api.on('terminate', cleanup.call))
    .add(api.cameraOn('positionchange', event => {
      chunkManager.updatePosition(event);
    }))
    .add(() => chunkManager.terminate())
  ;
};
