
const createBatchCall = require('../lib/batchCall');

const CROSSHAIR_SIZE = 40;

module.exports = async function plugin(registry) {
  const [engine, models, window] = await Promise.all(
      [registry.get('engine'), registry.get('models'), registry.get('window')]);

  const sprite = models.sprites['sprite.crosshair'];
  const crosshair = await engine.spriteInstanceCreate(sprite);

  const size = engine.windowGetSize(window.handle);
  update(size.width, size.height);

  const cleanup =
      createBatchCall().add(engine.windowOn(window.handle, 'resize', event => {
        update(event.width, event.height);
      }));

  function update(screenWidth, screenHeight) {
    const leftPadding = (screenWidth - CROSSHAIR_SIZE) / 2;
    const topPadding = (screenHeight - CROSSHAIR_SIZE) / 2;

    engine.spriteInstanceUpdate(crosshair, {
      x: leftPadding,
      y: topPadding,
      z: 0.1,
      scale: CROSSHAIR_SIZE,
      show: true
    });
  }

  return {
    onTerminate: cleanup
  }
}
