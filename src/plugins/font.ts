import { Font, Math } from "../chunklands.node";
import { PluginRegistry } from "../lib/plugin";
import { EnginePlugin } from "./engine";
import { RenderPipelinePlugin } from "./render-pipeline";

const {promisify} = require('util');
const fs = require('fs');
const readFile = promisify(fs.readFile);

interface IOpts {
  assetsDir: string
}

export interface FontPlugin {
  handle: bigint
  fontSize(string: string): Math.Size2D
}

export default async function fontPlugin(registry: PluginRegistry, opts: IOpts): Promise<FontPlugin> {

  const engine = await registry.get<EnginePlugin>('engine');
  await registry.get<RenderPipelinePlugin>('render_pipeline');
  const utf8 = await loadFont('OBICHO__')

  const font = await engine.fontLoad(utf8);

  async function loadFont(name: string): Promise<Font.Font> {
    const [meta, texture] = await Promise.all([
      readFile(`${opts.assetsDir}/fonts/${name}.json`),
      readFile(`${opts.assetsDir}/fonts/${name}.png`),
    ]);

    return {...JSON.parse(meta.toString()), texture};
  }

  return {
    handle: font,
    fontSize(string: string) {
      let height = 64;
      let width = 0;
      let lineWidth = 0;
      for (const char of string) {
        if (char === '\n' || char === '\r') {
          width = Math.max(width, lineWidth);
          lineWidth = 0;
          height += 64;
          continue;
        }

        const character = utf8.characters[char];
        if (character) {
          width += character.advance * (64 / utf8.size);
        }
      }

      return {width, height};
    }
  };
}
