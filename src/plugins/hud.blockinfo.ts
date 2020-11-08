import createBatchCall from "../lib/batchCall";
import { PluginRegistry } from "../lib/plugin";
import { EnginePlugin } from "./engine";
import { FontPlugin } from "./font";
import { WindowPlugin } from "./window";

const PADDING = 10;

export interface HudBlockInfoPlugin {
  onTerminate: () => Promise<void>
}

export default async function hudBlockInfoPlugin(registry: PluginRegistry) {
  const engine = await registry.get<EnginePlugin>('engine');
  const font = await registry.get<FontPlugin>('font');
  const window = await registry.get<WindowPlugin>('window');

  const fontHeight = font.fontSize('').height;
  const blockText = await engine.textCreate(font.handle);
  updatePosition(engine.windowGetContentSize(window.handle).height);

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
          .add(engine.windowOn(window.handle, 'contentresize', event => {
            updatePosition(event.height);
          }));

  function updatePosition(height: number) {
    engine.textUpdate(
        blockText, {pos: {x: PADDING, y: height - fontHeight - PADDING}});
  }

  function updateText(text: string) {
    engine.textUpdate(blockText, {text});
  }

  return {
    onTerminate: cleanup
  }
}
