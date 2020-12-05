import { Engine } from 'crankshaft-node-binding';
import { PluginRegistry } from '../lib/plugin';

export type EnginePlugin = Engine;

export default async function enginePlugin(registry: PluginRegistry): Promise<EnginePlugin> {
  const engine = new Engine();
  await engine.init()

  return engine;
}
