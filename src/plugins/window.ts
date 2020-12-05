import { Window } from 'crankshaft-node-binding';
import { PluginRegistry } from '../lib/plugin';
import { EnginePlugin } from './engine';

export interface WindowPlugin {
  onTerminate(): void;
  window: Window;
}

export default async function windowPlugin(
  registry: PluginRegistry
): Promise<WindowPlugin> {
  const engine = await registry.get<EnginePlugin>('engine');

  const window = await Window.create(engine, {width: 1024, height: 768, title: 'CHUNKLANDS'});
  await window.loadGL();

  window.onClose(async () => {
    await window.destroy();
    await engine.stop();
    engine.destroy();
  })

  return {
    onTerminate() {
    },
    window
  };
}
