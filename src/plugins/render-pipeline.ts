import { PluginRegistry } from '../lib/plugin';
import { EnginePlugin } from './engine';
import { WindowPlugin } from './window';

import { promisify } from 'util';
import fs from 'fs';
const readFile = promisify(fs.readFile);

interface IOpts {
  assetsDir: string;
}

export type RenderPipelinePlugin = void;

export default async function renderPipelinePlugin(
  registry: PluginRegistry,
  opts: IOpts
): Promise<RenderPipelinePlugin> {
  const engine = await registry.get<EnginePlugin>('engine');
  const window = await registry.get<WindowPlugin>('window');

  const [
    gbufferShader,
    lightingShader,
    blockSelectShader,
    spriteShader,
    textShader,
  ] = await Promise.all([
    loadShader('gbuffer'),
    loadShader('lighting'),
    loadShader('block_select'),
    loadShader('sprite'),
    loadShader('text'),
  ]);

  await engine.renderPipelineInit(window.handle, {
    gbuffer: gbufferShader,
    lighting: lightingShader,
    blockSelect: blockSelectShader,
    sprite: spriteShader,
    text: textShader,
  });

  async function loadShader(name: string) {
    const [vertexShader, fragmentShader] = await Promise.all([
      readFile(`${opts.assetsDir}/shader/${name}.vert`),
      readFile(`${opts.assetsDir}/shader/${name}.frag`),
    ]);

    return { vertexShader, fragmentShader };
  }
}
