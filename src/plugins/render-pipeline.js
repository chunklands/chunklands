
const { promisify } = require('util');
const fs = require('fs');
const readFile = promisify(fs.readFile);

module.exports = async function plugin(registry, { assetsDir }) {
  if (!assetsDir) {
    throw new Error('needs assets dir');
  }

  const [ engine, window, gbuffer, lighting ] = await Promise.all([
    registry.get('engine'),
    registry.get('window'),
    loadShader('gbuffer'),
    loadShader('lighting')
  ]);

  await engine.renderPipelineInit(window.handle, { gbuffer, lighting });

  async function loadShader(name) {
    const [vertexShader, fragmentShader] = await Promise.all([
      readFile(`${assetsDir}/shader/${name}.vert`),
      readFile(`${assetsDir}/shader/${name}.frag`),
    ]);
  
    return {
      vertexShader,
      fragmentShader
    };
  }
}

