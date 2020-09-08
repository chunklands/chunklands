
const createBatchCall = require('../lib/batchCall')
const PADDING = 10;

module.exports = async function plugin(registry) {
  const [engine, font, window] = await Promise.all(
      [registry.get('engine'), registry.get('font'), registry.get('window')]);

  const fontHeight = font.fontSize('').height;
  const blockText = await engine.textCreate(font.handle);
  updatePosition(engine.windowGetSize(window.handle).height);

  const cleanup =
      createBatchCall()
          .add(engine.gameOn(
              'pointingblockchange',
              event => {
                const text = event.pos ?
                    `x: ${event.pos.x}, y: ${event.pos.y}, z: ${event.pos.z}` :
                    '';
                updateText(text);
              }))
          .add(engine.windowOn(window.handle, 'resize', event => {
            updatePosition(event.height);
          }));

  function updatePosition(height) {
    engine.textUpdate(
        blockText, {pos: {x: PADDING, y: height - fontHeight - PADDING}});
  }

  function updateText(text) {
    engine.textUpdate(blockText, {text});
  }

  return {
    onTerminate: cleanup
  }
}
