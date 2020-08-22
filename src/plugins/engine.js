const debug = require('debug')('plugin:engine');

const DEBUG_ENGINE = false;
const DEBUG_PATTERN = /^chunkCreate$/

module.exports = async function plugin(registry, { chunklands }) {
  let engine = new chunklands.EngineBridge();

  if (DEBUG_ENGINE) {
    const Engine = Object.getPrototypeOf(engine);
    const engineMethods = Object.getOwnPropertyNames(Engine);
    const realEngine = engine;
    const engineProxy = {};
    for (const name of engineMethods) {
      const fn = engine[name].bind(engine);
      engineProxy[name] = (...args) => {
        if (DEBUG_PATTERN.test(name)) {
          debug(`%s(%o)`, name, args);
        }
        return realEngine[name](...args);
      };
    }

    engine = engineProxy;
  }

  return engine;
}