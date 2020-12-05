import { ModelsLoader } from '../models/types';
import { PluginRegistry } from '../lib/plugin';
import { EnginePlugin } from './engine';
import { RenderPipelinePlugin } from './render-pipeline';

interface IOpts {
  assetsDir: string;
  modelsLoader: ModelsLoader;
}

export type ModelsPlugin = void;

export default async function modelsPlugin(
  registry: PluginRegistry,
  opts: IOpts
): Promise<ModelsPlugin> {
  const engine = await registry.get<EnginePlugin>('engine');
  // TODO(daaitch): improper - needed for gbuffer to set texture id
  await registry.get<RenderPipelinePlugin>('render_pipeline');

  await loadModels();

  async function loadModels() {
    for (const model of await opts.modelsLoader({
      assetsDir: opts.assetsDir,
    })) {
      if (model.type === 'block') {
        await engine.blockNew(model);
      } else {
        console.warn(`unknown type: ${model.type}`)
      }
    }

  }
}
