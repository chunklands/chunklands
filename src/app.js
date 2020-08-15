const assert = require('assert');
const chunklands = require('../build/chunklands.node');
const { loadShader } = require('./lib/shader');
const ChunkManager = require('./lib/ChunkManager');

const engine = new chunklands.EngineBridge();
const api = new chunklands.EngineApiBridge(engine);

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

  const blocks = {};
  for (const block of await require('./assets/models')()) {
    const elements = Object.keys(block.faces).reduce((elements, face) => elements + block.faces[face].length, 0);
    const data = new ArrayBuffer(elements * Float32Array.BYTES_PER_ELEMENT);
    const floatData = new Float32Array(data);
    let i = 0;
    Object.keys(block.faces).forEach(face => {
      for (const d of block.faces[face]) {
        floatData[i] = d;
        i++;
      }
    });

    const blockHandle = await api.blockCreate({
      id: block.id,
      opaque: block.opaque,
      data,
      texture: block.texture
    });

    blocks[block.id] = blockHandle;
  }

  await api.blockBake();

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
