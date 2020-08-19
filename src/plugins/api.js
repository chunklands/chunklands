const debug = require('debug')('plugin:api');

const DEBUG_API = true;
const DEBUG_PATTERN = /^chunk.+/

module.exports = async function plugin(registry, opts) {
  const { chunklands } = opts;

  const engine = await registry.get('engine');
  
  let api = new chunklands.EngineApiBridge(engine);
  debug('created api')

  // if (debug) {
  //   const Api = Object.getPrototypeOf(api);
  //   const apiMethods = Object.getOwnPropertyNames(Api);
  //   const realApi = api;
  //   const apiProxy = {};
  //   for (const name of apiMethods) {
  //     const fn = api[name].bind(api);
  //     apiProxy[name] = (...args) => {
  //       if (DEBUG_PATTERN.test(name)) {
  //         console.log(`${name} [${args.join(', ')}]`);
  //       }
  //       return realApi[name](...args);
  //     };
  //   }

  //   api = apiProxy;
  // }

  return api;
}