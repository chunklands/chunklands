process.env.DEBUG = '*'

const chunklands = require('../build/chunklands.node');
const { createPluginRegistry } = require('./lib/plugin');

const registry = createPluginRegistry();
registry
  .load('engine', require('./plugins/engine'), {
    chunklands
  })
  .load('api', require('./plugins/api'), {
    chunklands
  })
  .load('profiler', require('./plugins/profiler'), {
    enable: true,
    profilesDir: `${__dirname}/../profiles`
  })
  .load('window', require('./plugins/window'))
  .load('render_pipeline', require('./plugins/render-pipeline'), {
    assetsDir: `${__dirname}/assets`
  })
  .load('world_generator', require('./plugins/world-generator'))
  .load('blocks', require('./plugins/blocks'), {
    modelLoader: require('./assets/models')
  })
  .load('camera_control', require('./plugins/camera-control'))
  .load('shutdown', require('./plugins/shutdown'), {
    async notifyTerminate() {
      await registry.invokeHook('onTerminate');
    }
  });
