const fs = require('fs');
const {WorldBase} = require('./module');

module.exports = class World extends WorldBase {
  constructor() {
    super({
      vertexShader: fs.readFileSync(`${__dirname}/game/shader/scene.vsh.glsl`),
      fragmentShader: fs.readFileSync(`${__dirname}/game/shader/scene.fsh.glsl`),
    });
  }
};
