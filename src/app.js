process.env.DEBUG = '*'

const chunklands = require('../build/chunklands.node');
const {createPluginRegistry} = require('./lib/plugin');

const assetsDir = `${__dirname}/assets`;

const registry = createPluginRegistry();
registry.load('engine', require('./plugins/engine'), {chunklands})
    .load('shutdown', require('./plugins/shutdown'), {
      async notifyTerminate() {
        await registry.invokeHook('onTerminate');
      }
    })
    .load(
        'profiler', require('./plugins/profiler'),
        {enable: true, profilesDir: `${__dirname}/../profiles`})
    .load('window', require('./plugins/window'))
    .load('render_pipeline', require('./plugins/render-pipeline'), {assetsDir})
    .load('world_generator', require('./plugins/world-generator'))
    .load(
        'models', require('./plugins/models'),
        {modelLoader: require('./assets/models')})
    .load('camera_control', require('./plugins/camera-control'))
    .load('font', require('./plugins/font'), {assetsDir})
    .load('hud.itemlist', require('./plugins/hud.itemlist'))
    .load('hud.blockinfo', require('./plugins/hud.blockinfo'))
    .load('hud.crosshair', require('./plugins/hud.crosshair'))
    .load('hud.version', require('./plugins/hud.version'));
