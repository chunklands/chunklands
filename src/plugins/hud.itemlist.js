
const createBatchCall = require('../lib/batchCall');

const LEFT_PADDING = 0;
const ITEM_PADDING = 0;

const ITEM_COUNT = 10;

module.exports = async function plugin(registry) {
  const [engine, models, window] = await Promise.all(
      [registry.get('engine'), registry.get('models'), registry.get('window')]);

  const sprite = models.sprites['sprite.itemlist-item'];
  const grass = models.sprites['sprite.block.grass'];

  const items = [];
  const itemBlocks = [];
  {
    const itemPromises = [];
    const itemBlockPromises = [];
    for (let i = 0; i < ITEM_COUNT; i++) {
      itemPromises.push(engine.spriteInstanceCreate(sprite));
      itemBlockPromises.push(engine.spriteInstanceCreate(grass))
    }

    items.push(...await Promise.all(itemPromises));
    itemBlocks.push(...await Promise.all(itemBlockPromises));
  }

  const size = engine.windowGetSize(window.handle);
  update(size.width);

  const cleanup =
      createBatchCall().add(engine.windowOn(window.handle, 'resize', event => {
        update(event.width);
      }));

  function update(screenWidth) {
    const itemWidth =
        ((screenWidth - 2 * LEFT_PADDING) - (ITEM_COUNT - 1) * ITEM_PADDING) /
        ITEM_COUNT;

    const blockPadding = itemWidth * 0.2;
    const blockWidth = itemWidth - 2 * blockPadding;

    const advance = ITEM_PADDING + itemWidth;

    for (let i = 0; i < ITEM_COUNT; i++) {
      const item = items[i];
      const x = LEFT_PADDING + (i * advance);
      const y = 0;
      engine.spriteInstanceUpdate(
          item, {x, y, z: 0.1, show: true, scale: itemWidth});

      const itemBlock = itemBlocks[i];
      engine.spriteInstanceUpdate(itemBlock, {
        x: x + blockPadding,
        y: y + blockPadding,
        z: 0.2,
        show: true,
        scale: blockWidth
      });
    }
  }

  return {onTerminate: cleanup};
}
