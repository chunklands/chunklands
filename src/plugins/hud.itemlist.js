
const createBatchCall = require('../lib/batchCall');

const LEFT_PADDING = 0;
const ITEM_PADDING = 0;

const ITEM_COUNT = 10;

module.exports = async function plugin(registry) {
  const [engine, models, window] = await Promise.all(
      [registry.get('engine'), registry.get('models'), registry.get('window')]);

  const sprite = models.sprites['sprite.itemlist-item'];
  const grass = models.sprites['sprite.block.grass'];

  let selectedItem = 0;

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

  const selection = await engine.spriteInstanceCreate(
      models.sprites['sprite.itemlist-selection']);

  let contentSize = engine.windowGetContentSize(window.handle);
  update();

  const cleanup = createBatchCall()
                      .add(engine.windowOn(
                          window.handle, 'contentresize',
                          event => {
                            contentSize = event;
                            update();
                          }))
                      .add(engine.windowOn(window.handle, 'scroll', event => {
                        selectedItem += event.y;
                        selectedItem = (selectedItem + ITEM_COUNT) % ITEM_COUNT;
                        update();
                      }));

  function update() {
    const itemWidth = ((contentSize.width - 2 * LEFT_PADDING) -
                       (ITEM_COUNT - 1) * ITEM_PADDING) /
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

      if (i === Math.round(selectedItem)) {
        engine.spriteInstanceUpdate(
            selection, {x, y, z: 0.15, show: true, scale: itemWidth});
      }

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
