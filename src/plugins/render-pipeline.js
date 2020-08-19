
const { promisify } = require('util');
const fs = require('fs');
const readFile = promisify(fs.readFile);

module.exports = async function plugin(registry, opts) {
  const { assetsDir } = opts;
  if (!assetsDir) {
    throw new Error('needs assets dir');
  }

  const [ api, window ]       = await Promise.all([registry.get('api'), registry.get('window')]);
  const [ gbuffer, lighting ] = await Promise.all([loadShader('gbuffer'), loadShader('lighting')]);

  await api.renderPipelineInit(window.handle, { gbuffer, lighting });

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

