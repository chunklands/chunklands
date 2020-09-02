
const {promisify} = require('util');
const fs = require('fs');
const readFile = promisify(fs.readFile);

module.exports = async function plugin(registry, {assetsDir}) {
  if (!assetsDir) {
    throw new Error('needs assets dir');
  }

  const [engine, window, gbuffer, lighting, blockSelect, sprite] =
      await Promise.all([
        registry.get('engine'), registry.get('window'), loadShader('gbuffer'),
        loadShader('lighting'), loadShader('block_select'), loadShader('sprite')
      ]);

  await engine.renderPipelineInit(
      window.handle, {gbuffer, lighting, blockSelect, sprite});

  async function loadShader(name) {
    const [vertexShader, fragmentShader] = await Promise.all([
      readFile(`${assetsDir}/shader/${name}.vert`),
      readFile(`${assetsDir}/shader/${name}.frag`),
    ]);

    return {vertexShader, fragmentShader};
  }
}
