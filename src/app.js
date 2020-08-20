process.env.DEBUG = '*'
const debug = require('debug')('app');
const chunklands = require('../build/chunklands.node');
const createPluginRegistry = require('./plugins/plugin');

(async () => {
  const registry = createPluginRegistry();
  registry
    .register('engine', require('./plugins/engine'), {
      chunklands
    })
    .register('api', require('./plugins/api'), {
      chunklands
    })
    .register('profiler', require('./plugins/profiler'), {
      enable: true,
      profilesDir: `${__dirname}/../profiles`
    })
    .register('window', require('./plugins/window'))
    .register('render_pipeline', require('./plugins/render-pipeline'), {
      assetsDir: `${__dirname}/assets`
    })
    .register('world_generator', require('./plugins/world-generator'))
    .register('blocks', require('./plugins/blocks'), {
      modelLoader: require('./assets/models')
    })
    .register('camera_control', require('./plugins/camera-control'))
    .register('shutdown', require('./plugins/shutdown'), {
      async notifyTerminate() {
        await registry.invokeHook('onTerminate');
      }
    })

  const [ api, window, engine ] = await Promise.all([
    registry.get('api'),
    registry.get('window'),
    registry.get('engine')
  ]);

})().catch(e => {
  console.error(e);
  process.exit(1);
});
