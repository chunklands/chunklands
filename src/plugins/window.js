const debug = require('debug')('plugin:window');

module.exports = async function plugin(registry) {
  const engine = await registry.get('engine');

  await engine.GLFWInit();

  const handle = await engine.windowCreate(1024, 768, 'chunklands');
  await engine.windowLoadGL(handle);

  let tid;
  function pollEvents() {
    tid = setTimeout(() => {
      engine.GLFWPollEvents();
      pollEvents();
    }, 10);
  }
  pollEvents();

  return {
    onTerminate() {
      clearTimeout(tid);
    },
    handle
  };
}