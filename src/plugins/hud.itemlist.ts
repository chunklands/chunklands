import createBatchCall from "../lib/batchCall";
import { PluginRegistry } from "../lib/plugin";
import { EnginePlugin } from "./engine";
import { ModelsPlugin } from "./models";
import { WindowPlugin } from "./window";

const LEFT_PADDING = 0;
const ITEM_PADDING = 0;

const ITEM_COUNT = 10;

export interface HudItemlistPlugin {
  onTerminate: () => Promise<void>
}

export default async function hudItemlistPlugin(registry: PluginRegistry): Promise<HudItemlistPlugin> {
  const engine = await registry.get<EnginePlugin>('engine');
  const models = await registry.get<ModelsPlugin>('models');
  const window = await registry.get<WindowPlugin>('window');

  const sprite = models.sprites['sprite.itemlist-item'];
  const grass = models.sprites['sprite.block.grass'];

  let selectedItem = 0;

  const items = [];
  const itemBlocks = [];
  {
    const itemPromises: Promise<bigint>[] = [];
    const itemBlockPromises: Promise<bigint>[] = [];
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
