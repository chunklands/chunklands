
const createBatchCall = require('../lib/batchCall');

const LEFT_PADDING = 0;
const ITEM_PADDING = 0;

module.exports = async function plugin(registry) {
  const [engine, models, window] = await Promise.all(
      [registry.get('engine'), registry.get('models'), registry.get('window')]);

  const sprite = models.sprites['sprite.itemlist-item'];

  const items = [];
  {
    const itemPromises = [];
    for (let i = 0; i < 10; i++) {
      itemPromises.push(engine.spriteInstanceCreate(sprite));
    }

    items.push(...await Promise.all(itemPromises));
  }

  const size = engine.windowGetSize(window.handle);
  update(size.width);

  const cleanup =
      createBatchCall().add(engine.windowOn(window.handle, 'resize', event => {
        update(event.width);
      }));

  function update(screenWidth) {
    const itemWidth =
        ((screenWidth - 2 * LEFT_PADDING) - (items.length - 1) * ITEM_PADDING) /
        items.length;

    const advance = ITEM_PADDING + itemWidth;

    for (let i = 0; i < items.length; i++) {
      const item = items[i];
      engine.spriteInstanceUpdate(item, {
        x: LEFT_PADDING + (i * advance),
        y: 0,
        show: true,
        scale: itemWidth
      });
    }
  }

  return {
    onTerminate: cleanup
  }
}
