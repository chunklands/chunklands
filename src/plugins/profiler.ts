import { PluginRegistry } from "../lib/plugin";
import { EnginePlugin } from "./engine";

interface IOptsDisabled {
  enable: undefined | false
}

interface IOptsEnabled {
  enable: true
  profilesDir: string
}

export interface ProfilerPlugin {
  onTerminate: () => void
}

export default async function profilerPlugin(registry: PluginRegistry, opts: IOptsDisabled | IOptsEnabled): Promise<ProfilerPlugin> {
  const engine = await registry.get<EnginePlugin>('engine');

  if (opts.enable) {
    engine.startProfiling();
  }

  return {
    onTerminate() {
      if (opts.enable) {
        engine.stopProfiling(`${opts.profilesDir}/profile-${Date.now()}.prof`);
      }
    }
  }
}