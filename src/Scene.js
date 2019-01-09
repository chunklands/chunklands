const fs = require('fs');
const {SceneBase} = require('./module');
const Window = require('./Window');

module.exports = class Scene {
  constructor() {
    this._scene = new SceneBase({
      vertexShader: fs.readFileSync(`${__dirname}/../resource/scene.vsh.glsl`),
      fragmentShader: fs.readFileSync(`${__dirname}/../resource/scene.fsh.glsl`),
    });
  }

  /**
   * @param {Window} window 
   */
  prepare(window) {
    this._scene.prepare(window._window);
  }

  /**
   * 
   * @param {number} diff 
   */
  render(diff) {
    this._scene.render(diff);
  }
}