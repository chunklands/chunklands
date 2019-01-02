const {SceneBase} = require('./module');

module.exports = class Scene {
  constructor() {
    this._scene = new SceneBase();
  }
  prepare() {
    this._scene.prepare();
  }

  render() {
    this._scene.render();
  }
}