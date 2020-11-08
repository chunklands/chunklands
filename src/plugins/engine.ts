import { EngineBridge } from '../chunklands.node';
import { PluginRegistry } from '../lib/plugin';
import d from 'debug';
const debug = d('plugin:engine');

const DEBUG_ENGINE = false;
const DEBUG_PATTERN = /./;

interface IOpts {
  engine: EngineBridge;
}

export type EnginePlugin = EngineBridge;

export default async function enginePlugin(
  registry: PluginRegistry,
  opts: IOpts
): Promise<EnginePlugin> {
  if (DEBUG_ENGINE) {
    return debugDecoration(opts.engine);
  }

  return opts.engine;
}

function debugDecoration(engine: EngineBridge) {
  // eslint-disable-next-line @typescript-eslint/no-explicit-any
  const eng = engine as any;

  const Engine = Object.getPrototypeOf(eng);
  const engineMethods = Object.getOwnPropertyNames(Engine);

  // eslint-disable-next-line @typescript-eslint/ban-types
  const engineProxy: { [method: string]: Function } = {};
  for (const name of engineMethods) {
    engineProxy[name] = (...args: unknown[]) => {
      if (DEBUG_PATTERN.test(name)) {
        debug(`%s(%o)`, name, args);
      }
      return eng[name](...args);
    };
  }

  return (engineProxy as unknown) as EngineBridge;
}
