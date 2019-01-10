const fs = require('fs');
const {SceneBase} = require('./module');

module.exports = class Scene extends SceneBase {
  constructor() {
    super({
      vertexShader: fs.readFileSync(`${__dirname}/../resource/scene.vsh.glsl`),
      fragmentShader: fs.readFileSync(`${__dirname}/../resource/scene.fsh.glsl`),
    });
  }
}