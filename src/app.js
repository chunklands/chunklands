const chunklands = require('../build/chunklands.node');

const api = new chunklands.EngineBridge();
enginePoller();

(async () => {
  await api.GLFWInit();
  api.GLFWStartPollEvents(true);
  const win = await api.windowCreate(200, 200, 'chunklands');
  api.windowOn(win, 'shouldclose', () => {
    console.log('OK')
    api.engineShutdown();
  });

  // cleanup();

  // setTimeout(() => {
  //   eb.engineShutdown()
  // }, 5000);
})();

function enginePoller() {
  if (api.enginePollEvents()) {
    setImmediate(enginePoller);
  }
}