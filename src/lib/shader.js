
const { promisify } = require('util');
const fs = require('fs');
const readFile = promisify(fs.readFile);

module.exports = {
  loadShader
};

async function loadShader(name) {
  const [vertexShaderSource, fragmentShaderSource] = await Promise.all([
    readFile(`${__dirname}/../assets/shader/${name}.vert`),
    readFile(`${__dirname}/../assets/shader/${name}.frag`),
  ]);

  return {
    vertexShaderSource,
    fragmentShaderSource
  };
}