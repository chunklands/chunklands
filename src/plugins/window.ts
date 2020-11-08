import { PluginRegistry } from '../lib/plugin';
import { EnginePlugin } from './engine';

export interface WindowPlugin {
  onTerminate(): void;
  handle: bigint;
}

export default async function windowPlugin(
  registry: PluginRegistry
): Promise<WindowPlugin> {
  const engine = await registry.get<EnginePlugin>('engine');

  await engine.GLFWInit();

  const handle = await engine.windowCreate(1024, 768, 'chunklands');
  await engine.windowLoadGL(handle);

  let tid: NodeJS.Timeout | undefined;
  // TODO(daaitch): start poll events, instead of timeout?
  function pollEvents() {
    tid = setTimeout(() => {
      engine.GLFWPollEvents();
      pollEvents();
    }, 10);
  }
  pollEvents();

  return {
    onTerminate() {
      if (tid !== undefined) {
        clearTimeout(tid);
      }
    },
    handle,
  };
}
