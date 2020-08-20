const process = require('process');
const createBatchCall = require('../lib/batchCall');

module.exports = async function plugin(registry, opts) {
  const { notifyTerminate } = opts;
  if (!(notifyTerminate instanceof Function)) {
    throw new Error('needs notifyTerminate');
  }

  const [ api, window, engine ] = await Promise.all([
    registry.get('api'),
    registry.get('window'),
    registry.get('engine')
  ]);

  const cleanup = createBatchCall()
    .add(api.windowOn(window.handle, 'shouldclose', () => {
      notifyTerminate().then(() => {
        engine.terminate();

        // for debugging open handles
        // console.log(process._getActiveHandles());
      })
    }));

  return {
    onTerminate: cleanup
  };
}