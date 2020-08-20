const { EventEmitter } = require('events');
const debug = require('debug');

class PluginRegistry {
  constructor() {
    this._ee = new EventEmitter();
    this._ee.setMaxListeners(50); // suppress memory leak warning
    this._plugins = new Map();

    this._ee.on('error', event => {
      console.error(`PLUGIN ERROR ${event.name}`, event.error);
    });

    this.name = 'root';
    this.debug = debug('plugin:root');
  }

  _fork(name) {
    const registry = Object.create(this);
    registry.name = name;
    registry.debug = debug(`plugin:${name}`);

    return registry;
  }

  get(pluginName) {
    this.debug('request "%s"', pluginName);
    if (this._plugins.has(pluginName)) {
      const { plugin } = this._plugins.get(pluginName);
      return Promise.resolve(plugin);
    }

    return new Promise(resolve => {
      const handler = event => {
        if (event.name === pluginName) {
          this._ee.off('pluginloaded', handler);
          resolve(event.plugin);
        }
      }

      this._ee.on('pluginloaded', handler);
    });
  }

  load(name, fn, opts) {
    if (typeof name !== 'string' || !name) {
      throw new Error('name must be a string and not empty');
    }

    if (typeof fn !== 'function') {
      throw new Error('fn must be a function');
    }

    opts = opts || {};

    this.debug('load "%s"', name);

    (async () => {
      let plugin;
      try {
        const registry = this._fork(name);
        plugin = await fn(registry, opts);
        this.debug('loaded "%s" successfully', name);

        this._plugins.set(name, {plugin, registry});
        this._ee.emit('pluginloaded', { name, plugin });
      } catch (e) {
        this.debug('loaded "%s" with error: %j', name, e);
        this._ee.emit('error', { name, error: e });
      }
    })();

    return this;
  }

  async invokeHook(hookName, ...args) {
    const hookResults = []
    for (const [name, {plugin, registry}] of this._plugins.entries()) {
      const hook = plugin?.[hookName];
      if (hook instanceof Function) {
        registry.debug('invoke hook "%s"', hookName);
        const hookResult = Promise
          .resolve(hook.apply(plugin, args))
          .then(result => ({name, result}));
        hookResults.push(hookResult);
      }
    }

    return Promise.all(hookResults)
  }
}

module.exports = function createPluginRegistry() {
  return new PluginRegistry();
};

// const api = 'API';
// const engine = 'ENGINE';

// const game = pluginRegistry({api, engine});
// game
//   .register('plugin1', plugin1, { options: true }) // OK: visitor pattern
//   .register('plugin2', plugin2)
//   .get('plugin1').then(plugin1 => plugin1.a())

// async function plugin1(registry, opts) {
//   const plugin2 = await registry.get('plugin2');

//   return {
//     a() {
//       plugin2.b();
//     }
//   };
// }

// function plugin2(registry, opts) {
//   return {
//     b() {
//       console.log('B');
//     }
//   };
// }