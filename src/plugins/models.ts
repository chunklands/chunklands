import { ModelsLoader } from '../models/types';
import { Block } from '../chunklands.node';
import { PluginRegistry } from '../lib/plugin';
import { EnginePlugin } from './engine';
import { RenderPipelinePlugin } from './render-pipeline';
interface IOpts {
  assetsDir: string;
  modelsLoader: ModelsLoader;
}

export type ModelsPlugin = Block.BakeResult;

export default async function modelsPlugin(
  registry: PluginRegistry,
  opts: IOpts
): Promise<ModelsPlugin> {
  const engine = await registry.get<EnginePlugin>('engine');
  // TODO(daaitch): improper - needed for gbuffer to set texture id
  await registry.get<RenderPipelinePlugin>('render_pipeline');

  const models = await loadModels();

  return models;

  async function loadModels() {
    for (const model of await opts.modelsLoader({
      assetsDir: opts.assetsDir,
    })) {
      if (model.type === 'block') {
        engine.blockCreate(model);
      }

      if (model.type === 'sprite') {
        engine.spriteCreate(model);
      }
    }

    return await engine.blockBake();
  }
}
