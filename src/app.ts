process.env.DEBUG = '*';

import fs from 'fs';
import { PluginRegistry } from './lib/plugin';
import enginePlugin from './plugins/engine';
import windowPlugin from './plugins/window';
import renderPipelinePlugin from './plugins/render-pipeline';
import { Log } from 'crankshaft-node-binding';

Log.setLevel(1);

const assetsDir = process.env.ASSETS_DIR ?? `${process.cwd()}/assets`;
if (!fs.existsSync(assetsDir)) {
  console.error(`Cannot find assets dir: "${assetsDir}".`);
  process.exit(1);
}

const registry = new PluginRegistry();

registry
  .load('engine', enginePlugin, undefined)
  .load('window', windowPlugin, undefined)
  .load('render_pipeline', renderPipelinePlugin, { assetsDir })