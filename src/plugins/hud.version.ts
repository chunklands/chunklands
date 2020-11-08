import createBatchCall from '../lib/batchCall';
import { PluginRegistry } from '../lib/plugin';
import { EnginePlugin } from './engine';
import { FontPlugin } from './font';
import { WindowPlugin } from './window';
import { version } from '../../package.json';

const TEXT = `🚀 Chunklands: ${version}`;
const PADDING = 10;

export interface HudVersionPlugin {
  onTerminate: () => Promise<void>;
}

export default async function hudVersionPlugin(
  registry: PluginRegistry
): Promise<HudVersionPlugin> {
  const engine = await registry.get<EnginePlugin>('engine');
  const font = await registry.get<FontPlugin>('font');
  const window = await registry.get<WindowPlugin>('window');

  const versionTextSize = font.fontSize(TEXT);
  const versionText = await engine.textCreate(font.handle);
  const contentSize = engine.windowGetContentSize(window.handle);
  update(contentSize.width, contentSize.height);

  const cleanup = createBatchCall().add(
    engine.windowOn(window.handle, 'contentresize', (event) => {
      update(event.width, event.height);
    })
  );

  function update(screenWidth: number, screenHeight: number) {
    engine.textUpdate(versionText, {
      text: TEXT,
      pos: {
        x: screenWidth - versionTextSize.width - PADDING,
        y: screenHeight - versionTextSize.height - PADDING,
      },
    });
  }

  return { onTerminate: cleanup };
}
