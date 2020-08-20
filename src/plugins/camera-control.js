const debug = require('debug')('plugin:camera_control');
const createBatchCall = require('../lib/batchCall');

module.exports = async function plugin(registry, opts) {

  const [ api, window ] = await Promise.all([registry.get('api'), registry.get('window')]);

  const cleanup = createBatchCall()
    .add(api.windowOn(window.handle, 'click', () => {
      api.cameraAttachWindow(window.handle);
    }))
    .add(api.windowOn(window.handle, 'key', event => {
      // ESC release
      if (event.key === 256 && event.action === 0) {
        api.cameraDetachWindow(window.handle);
      }
    }));
  
  return {
    onTerminate: cleanup.call
  };
}