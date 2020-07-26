const chunklands = require('../build/chunklands.node');
const { loadShader } = require('./lib/shader');

const engine = new chunklands.EngineBridge();
const api = new chunklands.EngineApiBridge(engine);

(async () => {
  await api.GLFWInit();
  api.GLFWStartPollEvents(true);
  const win = await api.windowCreate(200, 200, 'chunklands');
  await api.windowLoadGL(win);

  const shader = await loadShader('gbuffer');
  await api.gbufferPassInit(win, shader.vertexShaderSource, shader.fragmentShaderSource);

  api.windowOn(win, 'shouldclose', () => {
    console.log('OK');
    engine.terminate();
  });
})().catch(e => {
  console.error(e);
  process.exit(1);
});
