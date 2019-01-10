const {WindowBase} = require('./module');

module.exports = class Window extends WindowBase {
  constructor({width = 1024, height = 768, title = 'Chunklands'} = {}) {
    super({width, height, title});
  }
};