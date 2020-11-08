process.env.DEBUG = '*';

import fs from 'fs';
import chunklands from './chunklands.node';
import { PluginRegistry } from './lib/plugin';
import enginePlugin from './plugins/engine';
import shutdownPlugin from './plugins/shutdown';
import profilerPlugin from './plugins/profiler';
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
import modelsLoader from './models';

const assetsDir = process.env.ASSETS_DIR ?? `${process.cwd()}/assets`;
if (!fs.existsSync(assetsDir)) {
  console.error(`Cannot find assets dir: "${assetsDir}".`);
  process.exit(1);
}

const registry = new PluginRegistry();

registry
  .load('engine', enginePlugin, { engine: new chunklands.EngineBridge() })
  .load('shutdown', shutdownPlugin, {
    async notifyTerminate() {
      await registry.invokeHook('onTerminate');
    },
  })
  .load('profiler', profilerPlugin, {
    enable: true,
    profilesDir: `${__dirname}/../profiles`,
  })
  .load('window', windowPlugin, undefined)
  .load('render_pipeline', renderPipelinePlugin, { assetsDir })
  .load('world_generator', worldGeneratorPlugin, undefined)
  .load('models', modelsPlugin, { modelsLoader, assetsDir })
  .load('camera_control', pluginCameraControl, undefined)
  .load('font', fontPlugin, { assetsDir })
  .load('hud.itemlist', hudItemlistPlugin, undefined)
  .load('hud.blockinfo', hudBlockInfoPlugin, undefined)
  .load('hud.crosshair', hudCrosshairPlugin, undefined)
  .load('hud.version', hudVersionPlugin, undefined);
