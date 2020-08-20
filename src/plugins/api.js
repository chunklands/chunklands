const debug = require('debug')('plugin:api');

const DEBUG_API = false;
const DEBUG_PATTERN = /^chunkCreate$/

module.exports = async function plugin(registry, opts) {
  const { chunklands } = opts;

  const engine = await registry.get('engine');
  
  let api = new chunklands.EngineApiBridge(engine);

  if (DEBUG_API) {
    const Api = Object.getPrototypeOf(api);
    const apiMethods = Object.getOwnPropertyNames(Api);
    const realApi = api;
    const apiProxy = {};
    for (const name of apiMethods) {
      const fn = api[name].bind(api);
      apiProxy[name] = (...args) => {
        if (DEBUG_PATTERN.test(name)) {
          debug(`%s(%o)`, name, args);
        }
        return realApi[name](...args);
      };
    }

    api = apiProxy;
  }

  return api;
}