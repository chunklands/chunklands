const debug = require('debug')('plugin:camera_control');
const createBatchCall = require('../lib/batchCall');

module.exports = async function plugin(registry, opts) {
  const [engine, window] =
      await Promise.all([registry.get('engine'), registry.get('window')]);

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
                        }
                      }));

  return {onTerminate: cleanup};
}