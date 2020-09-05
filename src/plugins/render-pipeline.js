
const {promisify} = require('util');
const fs = require('fs');
const readFile = promisify(fs.readFile);

module.exports = async function plugin(registry, {assetsDir}) {
  if (!assetsDir) {
    throw new Error('needs assets dir');
  }

  const [engine, window, gbufferShader, lightingShader, blockSelectShader, spriteShader, textShader, ubuntuFont] =
      await Promise.all([
        registry.get('engine'), registry.get('window'), loadShader('gbuffer'),
        loadShader('lighting'), loadShader('block_select'),
        loadShader('sprite'), loadShader('text')
      ]);

  await engine.renderPipelineInit(window.handle, {
    gbuffer: gbufferShader,
    lighting: lightingShader,
    blockSelect: blockSelectShader,
    sprite: spriteShader,
    text: textShader
  });

  async function loadShader(name) {
    const [vertexShader, fragmentShader] = await Promise.all([
      readFile(`${assetsDir}/shader/${name}.vert`),
      readFile(`${assetsDir}/shader/${name}.frag`),
    ]);

    return {vertexShader, fragmentShader};
  }
}
