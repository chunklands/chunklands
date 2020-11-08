import createBatchCall from '../lib/batchCall';
import { PluginRegistry } from '../lib/plugin';
import { EnginePlugin } from './engine';
import { ModelsPlugin } from './models';
import { WindowPlugin } from './window';

const CROSSHAIR_SIZE = 40;

export interface HudCrosshairPlugin {
  onTerminate: () => Promise<void>;
}

export default async function hudCrosshairPlugin(
  registry: PluginRegistry
): Promise<HudCrosshairPlugin> {
  const engine = await registry.get<EnginePlugin>('engine');
  const models = await registry.get<ModelsPlugin>('models');
  const window = await registry.get<WindowPlugin>('window');

  const sprite = models.sprites['sprite.crosshair'];
  const crosshair = await engine.spriteInstanceCreate(sprite);

  const contentSize = engine.windowGetContentSize(window.handle);
  update(contentSize.width, contentSize.height);

  const cleanup = createBatchCall().add(
    engine.windowOn(window.handle, 'contentresize', (event) => {
      update(event.width, event.height);
    })
  );

  function update(screenWidth: number, screenHeight: number) {
    const leftPadding = (screenWidth - CROSSHAIR_SIZE) / 2;
    const topPadding = (screenHeight - CROSSHAIR_SIZE) / 2;

    engine.spriteInstanceUpdate(crosshair, {
      x: leftPadding,
      y: topPadding,
      z: 0.1,
      scale: CROSSHAIR_SIZE,
      show: true,
    });
  }

  return {
    onTerminate: cleanup,
  };
}
