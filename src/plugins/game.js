
const {promisify} = require('util');
const fs = require('fs');
const readFile = promisify(fs.readFile);
const createBatchCall = require('../lib/batchCall');

module.exports = async function plugin(registry, {assetsDir}) {
  if (!assetsDir) {
    throw new Error('needs assets dir');
  }

  const [engine, _renderPipeline, utf8] = await Promise.all([
    registry.get('engine'), registry.get('render_pipeline'), loadFont('utf8')
  ]);

  const font = await engine.fontLoad(utf8);

  const text = await engine.textCreate(font);
  await engine.textUpdate(text, {pos: {x: 80, y: 80}, text: '🚀🚀🚀🚀 Chunklands'});

  const blockText = await engine.textCreate(font);
  await engine.textUpdate(blockText, {pos: {x: 80, y: 150}, text: ''});

  const cleanup =
      createBatchCall().add(engine.gameOn('pointingblockchange', event => {
        const text = event.pos ?
            `x: ${event.pos.x}, y: ${event.pos.y}, z: ${event.pos.z}` :
            '';
        engine.textUpdate(blockText, {text});
      }));

  async function loadFont(name) {
    const [meta, texture] = await Promise.all([
      readFile(`${assetsDir}/fonts/${name}.json`),
      readFile(`${assetsDir}/fonts/${name}.png`),
    ]);

    return {...JSON.parse(meta.toString()), texture};
  }

  return {
    onTerminate: cleanup
  }
}
