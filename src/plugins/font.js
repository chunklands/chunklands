
const {promisify} = require('util');
const fs = require('fs');
const readFile = promisify(fs.readFile);

module.exports = async function plugin(registry, {assetsDir}) {
  if (!assetsDir) {
    throw new Error('needs assets dir');
  }

  const [engine, _renderPipeline, utf8] = await Promise.all([
    registry.get('engine'), registry.get('render_pipeline'),
    loadFont('OBICHO__')
  ]);

  const font = await engine.fontLoad(utf8);

  async function loadFont(name) {
    const [meta, texture] = await Promise.all([
      readFile(`${assetsDir}/fonts/${name}.json`),
      readFile(`${assetsDir}/fonts/${name}.png`),
    ]);

    return {...JSON.parse(meta.toString()), texture};
  }

  return {
    handle: font,
    fontSize(string) {
      let height = 64;
      let width = 0;
      let lineWidth = 0;
      for (const char of string) {
        if (char === '\n' || char === '\r') {
          width = Math.max(width, lineWidth);
          lineWidth = 0;
          height += 64;
          continue;
        }

        const character = utf8.characters[char];
        if (character) {
          width += character.advance * (64 / utf8.size);
        }
      }

      return {width, height};
    }
  };
}
