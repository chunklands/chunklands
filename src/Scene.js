const fs = require('fs');
const {SceneBase} = require('./module');

module.exports = class Scene {
  constructor() {
    this._scene = new SceneBase({
      vertexShader: fs.readFileSync(`${__dirname}/../resource/scene.vsh.glsl`),
      fragmentShader: fs.readFileSync(`${__dirname}/../resource/scene.fsh.glsl`),
    });
  }

  prepare() {
    this._scene.prepare();
  }

  render() {
    this._scene.render();
  }
}