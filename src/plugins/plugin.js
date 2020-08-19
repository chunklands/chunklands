const { EventEmitter } = require('events');
const debug = require('debug')('plugin');

class PluginRegistry {
  constructor() {
    this._ee = new EventEmitter();
    this._ee.setMaxListeners(50); // suppress memory leak warning
    this._plugins = new Map();

    this._ee.on('error', event => {
      console.error(`PLUGIN ERROR ${event.name}`, event.error);
    });
  }

  get(pluginName) {
    debug('get  %s', pluginName);
    if (this._plugins.has(pluginName)) {
      return Promise.resolve(this._plugins.get(pluginName));
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

  register(name, fn, opts) {
    if (typeof name !== 'string' || !name) {
      throw new Error('name must be a string and not empty');
    }

    if (typeof fn !== 'function') {
      throw new Error('fn must be a function');
    }

    opts = opts || {};

    debug('load %s', name);

    (async () => {
      let plugin;
      try {
        plugin = await fn(this, opts);
        debug('load %s DONE', name);

        this._plugins.set(name, plugin);
        this._ee.emit('pluginloaded', { name, plugin });
      } catch (e) {
        debug('load %s ERR', name);
        this._ee.emit('error', { name, error: e });
      }
    })();

    return this;
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