const engine = require('./engine_module');
const { EventEmitter} = require('events');
const TextureBaker = require('./TextureBaker');

function createWindow() {
  const window = new engine.Window();
  window.events = new EventEmitter();

  return window;
}

module.exports = {
  TextureBaker,
  ...engine,
  createWindow,
};
