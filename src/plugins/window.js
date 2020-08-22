const debug = require('debug')('plugin:window');

module.exports = async function plugin(registry) {
  const engine = await registry.get('engine');

  await engine.GLFWInit();

  engine.GLFWStartPollEvents(true);
  const handle = await engine.windowCreate(1024, 768, 'chunklands');
  await engine.windowLoadGL(handle);

  return {
    handle
  };
}