
const createBatchCall = require('../lib/batchCall')
const {version} = require('../../package.json');
const TEXT = `🚀 Chunklands: ${version}`;
const PADDING = 10;

module.exports = async function plugin(registry) {
  const [engine, font, window] = await Promise.all(
      [registry.get('engine'), registry.get('font'), registry.get('window')]);

  const versionTextSize = font.fontSize(TEXT);
  console.log({versionTextSize})
  const versionText = await engine.textCreate(font.handle);
  const size = engine.windowGetSize(window.handle);
  update(size.width, size.height);

  const cleanup =
      createBatchCall().add(engine.windowOn(window.handle, 'resize', event => {
        update(event.width, event.height);
      }));

  function update(screenWidth, screenHeight) {
    console.log({screenWidth, screenHeight})
    engine.textUpdate(versionText, {
      text: TEXT,
      pos: {
        x: screenWidth - versionTextSize.width - PADDING,
        y: screenHeight - versionTextSize.height - PADDING
      }
    });
  }

  return {onTerminate: cleanup};
}
