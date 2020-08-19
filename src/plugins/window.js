const debug = require('debug')('plugin:window');

module.exports = async function plugin(registry) {
  const api = await registry.get('api');

  await api.GLFWInit();

  api.GLFWStartPollEvents(true);
  const handle = await api.windowCreate(1024, 768, 'chunklands');
  await api.windowLoadGL(handle);

  return {
    handle
  };
}