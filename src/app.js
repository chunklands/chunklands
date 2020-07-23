const chunklands = require('../build/chunklands.node');

const api = new chunklands.EngineBridge();

(async () => {
  await api.GLFWInit();
  api.GLFWStartPollEvents(true);
  const win = await api.windowCreate(200, 200, 'chunklands');
  api.windowMakeContextCurrent(win);

  api.windowOn(win, 'shouldclose', () => {
    console.log('OK');
    api.engineShutdown();
  });
})();
