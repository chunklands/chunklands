const fs = require('fs');
const {WorldBase} = require('./module');

module.exports = class World extends WorldBase {
  constructor() {
    super({
      vertexShader: fs.readFileSync(`${__dirname}/../resource/scene.vsh.glsl`),
      fragmentShader: fs.readFileSync(`${__dirname}/../resource/scene.fsh.glsl`),
    });
  }
};
