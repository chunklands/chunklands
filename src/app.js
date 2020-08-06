const assert = require('assert');
const chunklands = require('../build/chunklands.node');
const { loadShader } = require('./lib/shader');

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
    console.log({elements});
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
    console.log({blockHandle});
    blocks[block.id] = blockHandle;
  }

  await api.blockBake();
  
  console.log({blocks})

  const chunk = await api.chunkCreate(0, 0, 0);
  await api.chunkUpdate(chunk, createChunk(blocks));
  await api.sceneAddChunk(chunk);
  // await api.sceneRemoveChunk(chunk);

  api.windowOn(win, 'shouldclose', () => {
    console.log('OK');
    engine.terminate();
    engine.stopProfiling(`${__dirname}/../profiles/profile-${Date.now()}.prof`);
  });
})().catch(e => {
  console.error(e);
  process.exit(1);
});

function createChunk(blocks) {
  const arrayBuffer = new ArrayBuffer(32 * 32 * 32 * BigUint64Array.BYTES_PER_ELEMENT);
  const buffer = new BigUint64Array(arrayBuffer);

  const dirtHandle = blocks['block.dirt'];
  assert(dirtHandle);

  for (let i = 0; i < 32 * 32 * 32; i++) {
    buffer[i] = dirtHandle;
  }

  return arrayBuffer;
}
