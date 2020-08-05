
const { promisify } = require('util');
const fs = require('fs');
const readFile = promisify(fs.readFile);

module.exports = {
  loadShader
};

async function loadShader(name) {
  const [vertexShader, fragmentShader] = await Promise.all([
    readFile(`${__dirname}/../assets/shader/${name}.vert`),
    readFile(`${__dirname}/../assets/shader/${name}.frag`),
  ]);

  return {
    vertexShader,
    fragmentShader
  };
}