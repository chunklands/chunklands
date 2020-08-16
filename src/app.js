const DEBUG_API = true;
const DEBUG_PATTERN = /^chunk.+/

const chunklands = require('../build/chunklands.node');
const { loadShader } = require('./lib/shader');
const ChunkManager = require('./lib/ChunkManager');
const blockLoader = require('./lib/blockLoader');

const engine = new chunklands.EngineBridge();
let api = new chunklands.EngineApiBridge(engine);

if (DEBUG_API) {
  const Api = Object.getPrototypeOf(api);
  const apiMethods = Object.getOwnPropertyNames(Api);
  const realApi = api;
  const apiProxy = {};
  for (const name of apiMethods) {
    const fn = api[name].bind(api);
    apiProxy[name] = (...args) => {
      if (DEBUG_PATTERN.test(name)) {
        console.log(`${name} [${args.join(', ')}]`);
      }
      return realApi[name](...args);
    };
  }

  api = apiProxy;
}

(async () => {
  engine.startProfiling();
  await api.GLFWInit();
  api.GLFWStartPollEvents(true);
  const win = await api.windowCreate(1024, 768, 'chunklands');
  await api.windowLoadGL(win);

  const [gbuffer, lighting] = await Promise.all([
    loadShader('gbuffer'),
    loadShader('lighting')
  ]);
  await api.renderPipelineInit(win, {
    gbuffer,
    lighting
  });

  const blocks = await blockLoader(api);

  new ChunkManager(api, blocks);

  api.windowOn(win, 'shouldclose', () => {
    engine.terminate();
    engine.stopProfiling(`${__dirname}/../profiles/profile-${Date.now()}.prof`);
  });

  api.windowOn(win, 'click', event => {
    api.cameraAttachWindow(win);
  });

  api.windowOn(win, 'key', event => {
    // ESC release
    if (event.key === 256 && event.action === 0) {
      api.cameraDetachWindow(win);
      return;
    }
  });

})().catch(e => {
  console.error(e);
  process.exit(1);
});
