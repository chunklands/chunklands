const BlockRegistrar  = require('./BlockRegistrar');
const ChunkGenerator  = require('./ChunkGenerator');
const Environment     = require('./Environment');
const GameLoop        = require('./GameLoop');
const GLProgram       = require('./GLProgram');
const GBufferPass     = require('./GBufferPass');
const LightingPass    = require('./LightingPass');
const Profiler        = require('./Profiler');
const SSAOBlurPass    = require('./SSAOBlurPass');
const SSAOPass        = require('./SSAOPass');
const Scene           = require('./Scene');
const SimpleWorldGen  = require('./game/world/SimpleWorldGen');
const Skybox          = require('./Skybox');
const SkyboxPass      = require('./SkyboxPass');
const Window          = require('./Window');
const World           = require('./World');
const blocks          = require('./game/blocks');

(async () => {

  Environment.initialize();

  const window = new Window();
  window.initialize({
    width: 640,
    height: 480,
    title: 'Chunklands'
  });

  // set opengl context
  window.makeContextCurrent();
  Environment.loadProcs();

  const blockRegistrar = new BlockRegistrar();
  for (const block of blocks) {
    blockRegistrar.addBlock(block);
  }
  await blockRegistrar.bake();

  const chunkGenerator = new ChunkGenerator();
  chunkGenerator.setBlockRegistrar(blockRegistrar);

  const worldGenerator = new SimpleWorldGen(blockRegistrar.getBlockIds());
  chunkGenerator.setWorldGenerator(worldGenerator);

  const world = new World();
  world.setChunkGenerator(chunkGenerator);

  const scene = new Scene();
  scene.setWorld(world);

  const gbufferShader = await GLProgram.create({
    vertexShader: `${__dirname}/game/shader/gbuffer.vsh.glsl`,
    fragmentShader: `${__dirname}/game/shader/gbuffer.fsh.glsl`
  });
  const gbufferPass = new GBufferPass();
  gbufferPass.setProgram(gbufferShader);
  scene.setGBufferPass(gbufferPass);

  const ssaoShader = await GLProgram.create({
    vertexShader: `${__dirname}/game/shader/ssao.vsh.glsl`,
    fragmentShader: `${__dirname}/game/shader/ssao.fsh.glsl`
  });
  const ssaoPass = new SSAOPass();
  ssaoPass.setProgram(ssaoShader);
  scene.setSSAOPass(ssaoPass);

  const ssaoBlurShader = await GLProgram.create({
    vertexShader: `${__dirname}/game/shader/ssaoblur.vsh.glsl`,
    fragmentShader: `${__dirname}/game/shader/ssaoblur.fsh.glsl`
  });
  const ssaoBlurPass = new SSAOBlurPass();
  ssaoBlurPass.setProgram(ssaoBlurShader)
  scene.setSSAOBlurPass(ssaoBlurPass);

  const lightingShader = await GLProgram.create({
    vertexShader: `${__dirname}/game/shader/lighting.vsh.glsl`,
    fragmentShader: `${__dirname}/game/shader/lighting.fsh.glsl`
  });
  const lightingPass = new LightingPass();
  lightingPass.setProgram(lightingShader);
  scene.setLightingPass(lightingPass);

  const skyboxShader = await GLProgram.create({
    vertexShader: `${__dirname}/game/shader/skybox.vsh.glsl`,
    fragmentShader: `${__dirname}/game/shader/skybox.fsh.glsl`
  });

  const skyboxPass = new SkyboxPass();
  skyboxPass.setProgram(skyboxShader);
  scene.setSkyboxPass(skyboxPass);

  const skybox = new Skybox();
  skybox.initialize(`${__dirname}/game/skyboxes/skyboxsun5deg2/`);
  scene.setSkybox(skybox);

  // TODO(daaitch): add preparation phase: this has to be at the end to update buffers
  scene.setWindow(window);

  const gameLoop = new GameLoop();
  gameLoop.setScene(scene);
  
  gameLoop.start();
  const loop = () => {
    gameLoop.loop();
    
    if (window.shouldClose()) {
      window.close();
      gameLoop.stop();
      Environment.terminate();
      process.exit(0);
      return;
    }

    setTimeout(loop, 0);
  };

  loop();

  const profiler = new Profiler();

  setInterval(() => {
    console.log(profiler.getMeassurements());
  }, 1000);
})().catch(e => {
  console.error(e);
  process.exit(1);
});