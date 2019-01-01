const {WindowBase} = require('./module');
const {EventEmitter} = require('events');

module.exports = class Window extends EventEmitter {
  constructor({width = 1024, height = 768, title = 'Chunklands'} = {}) {
    super();
    this._window = new WindowBase({width, height, title});
  }

  get shouldClose() {
    return this._window.shouldClose;
  }

  close() {
    this._window.close();
  }

  makeContextCurrent() {
    this._window.makeContextCurrent();
  }

  swapBuffers() {
    this._window.swapBuffers();
  }

  clear() {
    this._window.clear();
  }
}