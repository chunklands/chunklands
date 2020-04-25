const BlockRegistrar  = require('./BlockRegistrar');
const ChunkGenerator  = require('./ChunkGenerator');
const Environment     = require('./Environment');
const GameLoop        = require('./GameLoop');
const GLProgram       = require('./GLProgram');
const GBufferPass     = require('./GBufferPass');
const Profiler        = require('./Profiler');
const SSAOPass        = require('./SSAOPass');
const Scene           = require('./Scene');
const SimpleWorldGen  = require('./game/world/SimpleWorldGen');
const Skybox          = require('./Skybox');
const SkyboxPass      = require('./SkyboxPass');
const Window          = require('./Window');
const World           = require('./World');

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
  blockRegistrar.addBlock(require('./game/blocks/coal'));
  blockRegistrar.addBlock(require('./game/blocks/cobblestone'));
  blockRegistrar.addBlock(require('./game/blocks/dirt'));
  blockRegistrar.addBlock(require('./game/blocks/gold'));
  blockRegistrar.addBlock(require('./game/blocks/grass'));
  blockRegistrar.addBlock(require('./game/blocks/iron'));
  blockRegistrar.addBlock(require('./game/blocks/sand'));
  blockRegistrar.addBlock(require('./game/blocks/stone'));
  blockRegistrar.addBlock(require('./game/blocks/water'));
  blockRegistrar.addBlock(require('./game/blocks/wood'));
  blockRegistrar.addBlock(require('./game/blocks/air'));
  await blockRegistrar.bake();

  const chunkGenerator = new ChunkGenerator();
  chunkGenerator.setBlockRegistrar(blockRegistrar);

  const worldGenerator = new SimpleWorldGen(blockRegistrar.getBlockIds());
  chunkGenerator.setWorldGenerator(worldGenerator);

  const world = new World();
  world.setChunkGenerator(chunkGenerator);

  const gbufferShader = await GLProgram.create({
    vertexShader: `${__dirname}/game/shader/gbuffer.vsh.glsl`,
    fragmentShader: `${__dirname}/game/shader/gbuffer.fsh.glsl`
  });
  const gbufferPass = new GBufferPass();
  gbufferPass.setProgram(gbufferShader);
  world.setGBufferPass(gbufferPass);

  const ssaoShader = await GLProgram.create({
    vertexShader: `${__dirname}/game/shader/ssao.vsh.glsl`,
    fragmentShader: `${__dirname}/game/shader/ssao.fsh.glsl`
  });
  const ssaoPass = new SSAOPass();
  ssaoPass.setProgram(ssaoShader);
  world.setSSAOPass(ssaoPass);

  const ssaoBlurShader = await GLProgram.create({
    vertexShader: `${__dirname}/game/shader/ssaoblur.vsh.glsl`,
    fragmentShader: `${__dirname}/game/shader/ssaoblur.fsh.glsl`
  });
  world.setSSAOBlurShader(ssaoBlurShader);

  const lightingShader = await GLProgram.create({
    vertexShader: `${__dirname}/game/shader/lighting.vsh.glsl`,
    fragmentShader: `${__dirname}/game/shader/lighting.fsh.glsl`
  });
  world.setLightingShader(lightingShader);

  const skyboxShader = await GLProgram.create({
    vertexShader: `${__dirname}/game/shader/skybox.vsh.glsl`,
    fragmentShader: `${__dirname}/game/shader/skybox.fsh.glsl`
  });

  const skyboxPass = new SkyboxPass();
  skyboxPass.useProgram(skyboxShader)
  
  world.setSkyboxPass(skyboxPass);

  const skybox = new Skybox();
  skybox.initialize(`${__dirname}/game/skyboxes/skyboxsun5deg2/`);
  world.setSkybox(skybox);

  const scene = new Scene();
  scene.setWindow(window);
  scene.setWorld(world);

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