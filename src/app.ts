process.env.DEBUG = '*'

import chunklands from './chunklands.node';
import {PluginRegistry} from './lib/plugin';
import enginePlugin from './plugins/engine';
import shutdownPlugin from './plugins/shutdown';
import profilerPlugin from './plugins/profiler'
import windowPlugin from './plugins/window';
import renderPipelinePlugin from './plugins/render-pipeline';
import worldGeneratorPlugin from './plugins/world-generator';
import modelsPlugin from './plugins/models';
import pluginCameraControl from './plugins/camera-control';
import fontPlugin from './plugins/font';
import hudItemlistPlugin from './plugins/hud.itemlist';
import hudBlockInfoPlugin from './plugins/hud.blockinfo';
import hudCrosshairPlugin from './plugins/hud.crosshair';
import hudVersionPlugin from './plugins/hud.version';

import modelsLoader from './models'

const assetsDir = `${__dirname}/../assets`;
const registry = new PluginRegistry();

registry
  .load('engine', enginePlugin, {engine: new chunklands.EngineBridge()})
  .load('shutdown', shutdownPlugin, {
    async notifyTerminate() {
      await registry.invokeHook('onTerminate');
    }
  })
  .load('profiler', profilerPlugin, {enable: true, profilesDir: `${__dirname}/../profiles`})
  .load('window', windowPlugin)
  .load('render_pipeline', renderPipelinePlugin, {assetsDir})
  .load('world_generator', worldGeneratorPlugin)
  .load('models', modelsPlugin, {modelsLoader, assetsDir})
  .load('camera_control', pluginCameraControl)
  .load('font', fontPlugin, {assetsDir})
  .load('hud.itemlist', hudItemlistPlugin)
  .load('hud.blockinfo', hudBlockInfoPlugin)
  .load('hud.crosshair', hudCrosshairPlugin)
  .load('hud.version', hudVersionPlugin);
