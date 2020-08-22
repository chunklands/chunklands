const createBatchCall = require('../lib/batchCall');

module.exports = async function plugin(registry, { notifyTerminate }) {
  if (!(notifyTerminate instanceof Function)) {
    throw new Error('needs notifyTerminate');
  }

  const [ engine, window ] = await Promise.all([
    registry.get('engine'),
    registry.get('window')
  ]);

  const cleanup = createBatchCall()
    .add(engine.windowOn(window.handle, 'shouldclose', () => {
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