import createBatchCall from '../lib/batchCall';
import { PluginRegistry } from '../lib/plugin';
import { EnginePlugin } from './engine';
import { WindowPlugin } from './window';

interface IOpts {
  notifyTerminate: () => Promise<void>;
}

export interface ShutdownPlugin {
  onTerminate: () => Promise<void>;
}

export default async function shutdownPlugin(
  registry: PluginRegistry,
  opts: IOpts
) {
  const engine = await registry.get<EnginePlugin>('engine');
  const window = await registry.get<WindowPlugin>('window');

  const cleanup = createBatchCall().add(
    engine.windowOn(window.handle, 'shouldclose', () => {
      opts.notifyTerminate().then(() => {
        engine.terminate();

        // for debugging open handles
        // console.log(process._getActiveHandles());

        process.exit(0);
      });
    })
  );

  return { onTerminate: cleanup };
}
