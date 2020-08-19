const debug = require('debug')('plugin:engine');

module.exports = function plugin(registry, opts) {
  const { chunklands } = opts;

  const engine = new chunklands.EngineBridge();
  debug('created engine');
  return engine;
}