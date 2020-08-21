const debug = require('debug')('plugin:engine');

module.exports = function plugin(registry, { chunklands }) {
  const engine = new chunklands.EngineBridge();
  return engine;
}