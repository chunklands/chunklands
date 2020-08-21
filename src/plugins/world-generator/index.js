const debug = require('debug')('world_generator');
const ChunkManager = require('./ChunkManager');
const createBatchCall = require('../../lib/batchCall');
const Abort = require('../../lib/Abort');

module.exports = async function worldGenerator(registry, opts) {
  const [ api, blocks ] = await Promise.all([
    registry.get('api'),
    registry.get('blocks')
  ]);

  const abort = new Abort();

  const chunkManager = new ChunkManager({abort, api, blocks});
  api.cameraGetPosition().then(cameraPos => chunkManager.updatePosition(abort, cameraPos));

  const cleanup = createBatchCall()
    .add(api.cameraOn('positionchange', event => {
      chunkManager.updatePosition(abort, event);
    }))
    .add(() => abort.abort())
    .call;

  return {
    onTerminate: cleanup
  };
};
