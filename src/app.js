process.env.DEBUG = '*'
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
      enable: true
    })
    .register('window', require('./plugins/window'))
    .register('render_pipeline', require('./plugins/render-pipeline'), {
      assetsDir: `${__dirname}/assets`
    })
    .register('world_generator', require('./plugins/world-generator'))
    .register('blocks', require('./plugins/blocks'), {
      modelLoader: require('./assets/models')
    })

  const [ api, window, engine ] = await Promise.all([
    registry.get('api'),
    registry.get('window'),
    registry.get('engine')
  ]);

  api.windowOn(window.handle, 'shouldclose', () => {
    engine.terminate();
    engine.stopProfiling(`${__dirname}/../profiles/profile-${Date.now()}.prof`);
  });

  api.windowOn(window.handle, 'click', event => {
    api.cameraAttachWindow(window.handle);
  });

  api.windowOn(window.handle, 'key', event => {
    // ESC release
    if (event.key === 256 && event.action === 0) {
      api.cameraDetachWindow(window.handle);
      return;
    }
  });

})().catch(e => {
  console.error(e);
  process.exit(1);
});
