const chunklands = require('../build/chunklands.node');

const engine = new chunklands.EngineBridge();
const api = new chunklands.EngineApiBridge(engine);

engine.init();

(async () => {
  await api.GLFWInit();
  api.GLFWStartPollEvents(true);
  const win = await api.windowCreate(200, 200, 'chunklands');
  api.windowLoadGL(win);

  api.windowOn(win, 'shouldclose', () => {
    console.log('OK');
    api.terminate();
    engine.shutdown();
  });
})().catch(e => {
  console.error(e);
  process.exit(1);
});
