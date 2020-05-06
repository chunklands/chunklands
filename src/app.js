const chunklands      = require('./chunklands');
const SimpleWorldGen  = require('./game/world/SimpleWorldGen');
const blocks          = require('./game/blocks');

(async () => {

  chunklands.engine.Environment.initialize();

  const window = new chunklands.engine.Window();
  window.initialize({
    width: 640,
    height: 480,
    title: 'Chunklands'
  });

  // set opengl context
  window.makeContextCurrent();
  chunklands.engine.Environment.loadProcs();

  const blockRegistrar = new chunklands.game.BlockRegistrar();
  for (const block of blocks) {
    blockRegistrar.addBlock(block);
  }
  await blockRegistrar.bake();

  const chunkGenerator = new chunklands.game.ChunkGenerator();
  chunkGenerator.setBlockRegistrar(blockRegistrar);

  const worldGenerator = new SimpleWorldGen(blockRegistrar.getBlockIds());
  chunkGenerator.setWorldGenerator(worldGenerator);

  const world = new chunklands.game.World();
  world.setChunkGenerator(chunkGenerator);

  const scene = new chunklands.game.Scene();
  scene.setWorld(world);

  const gbufferShader = await chunklands.gl.Program.create({
    vertexShader: `${__dirname}/game/shader/gbuffer.vsh.glsl`,
    fragmentShader: `${__dirname}/game/shader/gbuffer.fsh.glsl`
  });
  const gbufferPass = new chunklands.engine.GBufferPass();
  gbufferPass.setProgram(gbufferShader);
  scene.setGBufferPass(gbufferPass);

  const ssaoShader = await chunklands.gl.Program.create({
    vertexShader: `${__dirname}/game/shader/ssao.vsh.glsl`,
    fragmentShader: `${__dirname}/game/shader/ssao.fsh.glsl`
  });
  const ssaoPass = new chunklands.engine.SSAOPass();
  ssaoPass.setProgram(ssaoShader);
  scene.setSSAOPass(ssaoPass);

  const ssaoBlurShader = await chunklands.gl.Program.create({
    vertexShader: `${__dirname}/game/shader/ssaoblur.vsh.glsl`,
    fragmentShader: `${__dirname}/game/shader/ssaoblur.fsh.glsl`
  });
  const ssaoBlurPass = new chunklands.engine.SSAOBlurPass();
  ssaoBlurPass.setProgram(ssaoBlurShader)
  scene.setSSAOBlurPass(ssaoBlurPass);

  const lightingShader = await chunklands.gl.Program.create({
    vertexShader: `${__dirname}/game/shader/lighting.vsh.glsl`,
    fragmentShader: `${__dirname}/game/shader/lighting.fsh.glsl`
  });
  const lightingPass = new chunklands.engine.LightingPass();
  lightingPass.setProgram(lightingShader);
  scene.setLightingPass(lightingPass);

  const skyboxShader = await chunklands.gl.Program.create({
    vertexShader: `${__dirname}/game/shader/skybox.vsh.glsl`,
    fragmentShader: `${__dirname}/game/shader/skybox.fsh.glsl`
  });

  const skyboxPass = new chunklands.engine.SkyboxPass();
  skyboxPass.setProgram(skyboxShader);
  scene.setSkyboxPass(skyboxPass);

  const skybox = new chunklands.engine.Skybox();
  skybox.initialize(`${__dirname}/game/skyboxes/skyboxsun5deg2/`);
  scene.setSkybox(skybox);

  // TODO(daaitch): add preparation phase: this has to be at the end to update buffers
  scene.setWindow(window);

  const gameLoop = new chunklands.engine.GameLoop();
  gameLoop.setScene(scene);
  
  gameLoop.start();
  const loop = () => {
    gameLoop.loop();
    
    if (window.shouldClose()) {
      window.close();
      gameLoop.stop();
      chunklands.engine.Environment.terminate();
      process.exit(0);
      return;
    }

    setTimeout(loop, 0);
  };

  loop();

  const profiler = new chunklands.misc.Profiler();

  setInterval(() => {
    console.log(profiler.getMeassurements());
  }, 1000);
})().catch(e => {
  console.error(e);
  process.exit(1);
});