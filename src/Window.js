const {WindowBase} = require('./module');

module.exports = class Window extends WindowBase {
  initialize({width = 1024, height = 768, title = 'Chunklands'} = {}) {
    super.initialize({width, height, title});
  }
};