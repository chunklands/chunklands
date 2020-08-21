const createBatchCall = require('../lib/batchCall');

/**
 * @param {*} registry 
 * @param {{notifyTerminate}} param1 
 */
module.exports = async function plugin(registry, { notifyTerminate }) {
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