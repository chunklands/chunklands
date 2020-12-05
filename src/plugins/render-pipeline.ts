import { PluginRegistry } from '../lib/plugin';
import { WindowPlugin } from './window';

import { promisify } from 'util';
import fs from 'fs';
import { Renderpipeline } from 'crankshaft-node-binding';
const readFile = promisify(fs.readFile);

interface IOpts {
  assetsDir: string;
}

export type RenderPipelinePlugin = void;

export default async function renderPipelinePlugin(
  registry: PluginRegistry,
  opts: IOpts
): Promise<RenderPipelinePlugin> {
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

  await Renderpipeline.init(window.window.handle, {
    gbufferVertexShader: gbufferShader.vertexShader,
    gbufferFragmentShader: gbufferShader.fragmentShader,
    lightingVertexShader: lightingShader.vertexShader,
    lightingFragmentShader: lightingShader.fragmentShader,
    selectblockVertexShader: blockSelectShader.vertexShader,
    selectblockFragmentShader: blockSelectShader.fragmentShader,
    spriteVertexShader: spriteShader.vertexShader,
    spriteFragmentShader: spriteShader.fragmentShader,
    textVertexShader: textShader.vertexShader,
    textFragmentShader: textShader.fragmentShader
  });

  async function loadShader(name: string) {
    const [vertexShader, fragmentShader] = await Promise.all([
      readFile(`${opts.assetsDir}/shader/${name}.vert`),
      readFile(`${opts.assetsDir}/shader/${name}.frag`),
    ]);

    return {
      vertexShader: vertexShader.toString(),
      fragmentShader: fragmentShader.toString()
    };
  }
}
