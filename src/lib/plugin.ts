import { EventEmitter } from 'events';
import debug from 'debug';

export class PluginRegistry {
  private ee = new EventEmitter();
  private plugins = new Map();

  private name = 'root';
  private debug = debug('plugin:root');

  constructor() {
    this.ee.setMaxListeners(50); // suppress memory leak warning
    this.ee.on('error', (event) => {
      console.error(`PLUGIN ERROR ${event.name}`, event.error);
      process.exit(1);
    });
  }

  private fork(name: string) {
    const registry = Object.create(this) as PluginRegistry;
    registry.name = name;
    registry.debug = debug(`plugin:${name}`);

    return registry;
  }

  get<T>(pluginName: string): Promise<T> {
    this.debug('request "%s"', pluginName);
    if (this.plugins.has(pluginName)) {
      const { plugin } = this.plugins.get(pluginName);
      return Promise.resolve<T>(plugin);
    }

    return new Promise<T>((resolve) => {
      const handler = (event: { name: string; plugin: T }) => {
        if (event.name === pluginName) {
          this.ee.off('pluginloaded', handler);
          resolve(event.plugin);
        }
      };

      this.ee.on('pluginloaded', handler);
    });
  }

  load<T>(
    name: string,
    fn: (registry: PluginRegistry, opts: T) => unknown,
    opts: T
  ) {
    this.debug('load "%s"', name);

    (async () => {
      let plugin;
      try {
        const registry = this.fork(name);
        plugin = await fn(registry, opts);
        this.debug('loaded "%s" successfully', name);

        this.plugins.set(name, { plugin, registry });
        this.ee.emit('pluginloaded', { name, plugin });
      } catch (e) {
        this.debug('loaded "%s" with error: %j', name, e);
        this.ee.emit('error', { name, error: e });
      }
    })();

    return this;
  }

  async invokeHook(hookName: string, ...args: unknown[]) {
    const hookResults = [];
    for (const [name, { plugin, registry }] of this.plugins.entries()) {
      const hook = plugin?.[hookName];
      if (hook instanceof Function) {
        registry.debug('invoke hook "%s"', hookName);
        const hookResult = Promise.resolve(
          hook.apply(plugin, args)
        ).then((result) => ({ name, result }));
        hookResults.push(hookResult);
      }
    }

    return Promise.all(hookResults);
  }
}
