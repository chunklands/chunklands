import createBatchCall from "../lib/batchCall";
import { PluginRegistry } from "../lib/plugin";
import { EnginePlugin } from "./engine";
import { WindowPlugin } from "./window";

interface CameraControlPlugin {
  onTerminate: () => Promise<void>
}

export default async function pluginCameraControl(registry: PluginRegistry): Promise<CameraControlPlugin> {
  const engine = await registry.get<EnginePlugin>('engine');
  const window = await registry.get<WindowPlugin>('window');

  const cleanup = createBatchCall()
                      .add(engine.windowOn(
                          window.handle, 'click',
                          () => {
                            engine.cameraAttachWindow(window.handle);
                          }))
                      .add(engine.windowOn(window.handle, 'key', event => {
                        // ESC release
                        if (event.key === 256 && event.action === 0) {
                          engine.cameraDetachWindow(window.handle);
                        } else if (event.key === 82 && event.action === 1) {
                          const collision = engine.characterIsCollision();
                          engine.characterSetCollision(!collision)
                        } else if (event.key === 70 && event.action === 1) {
                          const flightMode = engine.characterIsFlightMode();
                          engine.characterSetFlightMode(!flightMode);
                        }
                      }));

  return {onTerminate: cleanup};
}