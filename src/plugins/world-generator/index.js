const debug = require('debug')('world_generator');
const ChunkManager = require('./ChunkManager');
const createBatchCall = require('../../lib/batchCall');
const Abort = require('../../lib/Abort');

module.exports = async function worldGenerator(registry, opts) {
  const [ engine, blocks ] = await Promise.all([
    registry.get('engine'),
    registry.get('blocks')
  ]);

  const abort = new Abort();

  const chunkManager = new ChunkManager({abort, engine, blocks});
  engine.cameraGetPosition().then(cameraPos => chunkManager.updatePosition(abort, cameraPos));

  const cleanup = createBatchCall()
    .add(engine.cameraOn('positionchange', event => {
      chunkManager.updatePosition(abort, event);
    }))
    .add(() => abort.abort());

  return {
    onTerminate: cleanup
  };
};
