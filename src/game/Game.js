const fs = require('fs');
const { promisify } = require('util');
const { engine, game, gl, misc } = require('../chunklands');
const SimpleWorldGen = require('./world/SimpleWorldGen')
const modelLoader    = require('./models');
const files = require('../files');

module.exports = class Game {
  /**
   * @param {import('../chunklands/engine/_').Window} window 
   */
  constructor(window) {
    this._window = window;
  }

  async initialize() {


    // blocks
    const models = await modelLoader();
    const bakedModels = await game.bakeModels(models);

    const blockRegistrar = new game.BlockRegistrar();
    const spriteRegistrar = new game.SpriteRegistrar();

    this._blockIds = {};
    for (const bakedModel of bakedModels.models) {
      if (bakedModel.id.startsWith('block.')) {
        const blockId = blockRegistrar.addBlock(bakedModel);
        this._blockIds[bakedModel.id] = blockId;
      } else if (bakedModel.id.startsWith('sprite.')) {
        spriteRegistrar.addSprite(bakedModel);
      }
    }

    const texturePath = files.cacheFile('texture.png');
    await bakedModels.texture.writeAsync(texturePath)
    blockRegistrar.loadTexture(texturePath);

    const chunkGenerator = new game.ChunkGenerator();
    chunkGenerator.setBlockRegistrar(blockRegistrar);

    
    // world

    const worldGenerator = new SimpleWorldGen(this._blockIds);
    chunkGenerator.setWorldGenerator(worldGenerator);

    this._world = new game.World();
    this._world.setChunkGenerator(chunkGenerator);
    this._world.setBlockRegistrar(blockRegistrar);

    // game overlay
    const gameOverlay = new game.GameOverlay();
    gameOverlay.setSpriteRegistrar(spriteRegistrar);

    // scene
    const scene = game.createScene();
    scene.setGameOverlay(gameOverlay);
    scene.setBlockRegistrar(blockRegistrar);
    scene.setWorld(this._world);


    // render pipeline
    {
      const gbufferPass = new engine.GBufferPass();
      gbufferPass.setProgram(await createProgram('gbuffer'));
      scene.setGBufferPass(gbufferPass);
    }

    {
      const ssaoPass = new engine.SSAOPass();
      ssaoPass.setProgram(await createProgram('ssao'));
      scene.setSSAOPass(ssaoPass);
    }

    {
      const ssaoBlurPass = new engine.SSAOBlurPass();
      ssaoBlurPass.setProgram(await createProgram('ssaoblur'));
      scene.setSSAOBlurPass(ssaoBlurPass);
    }

    {
      const lightingPass = new engine.LightingPass();
      lightingPass.setProgram(await createProgram('lighting'));
      scene.setLightingPass(lightingPass);
    }

    {
      const skyboxPass = new engine.SkyboxPass();
      skyboxPass.setProgram(await createProgram('skybox'));
      scene.setSkyboxPass(skyboxPass);
    }


    // skybox
    {
      const skybox = new engine.Skybox();
      skybox.initialize(`${__dirname}/skyboxes/skybox_day/`);
      scene.setSkybox(skybox);
    }

    // game overlay
    {
      const gameOverlayRenderer = new engine.GameOverlayRenderer();
      gameOverlayRenderer.setProgram(await createProgram('game_overlay'));
      scene.setGameOverlayRenderer(gameOverlayRenderer);
    }

    // block select
    {
      const blockSelectPass = new engine.BlockSelectPass();
      blockSelectPass.setProgram(await createProgram('block_select'));
      scene.setBlockSelectPass(blockSelectPass);
    }


    // camera
    this._camera = new engine.Camera();
    this._camera.setPosition(0, 30, 0);
    scene.setCamera(this._camera);


    // movement controller
    const movementController = new engine.MovementController();
    movementController.setCamera(this._camera);
    movementController.setCollisionSystem(this._world);
    scene.setMovementController(movementController);

    // text renderer
    const fontLoader = await createFontLoader('ubuntu');
    const textRenderer = new engine.TextRenderer();
    textRenderer.setFontLoader(fontLoader);
    textRenderer.setProgram(await createProgram('text'));
    scene.setTextRenderer(textRenderer);


    // TODO(daaitch): add preparation phase: this has to be at the end to update buffers
    scene.setWindow(this._window);

    // gameloop

    const gameLoop = new engine.GameLoop();
    gameLoop.setScene(scene);

    this._scene = scene;
    this._gameLoop = gameLoop;
    this._textRenderer = textRenderer;
  }

  async run() {
    this._window.events.on('key', event => {
      if (event.key === 70 && event.action === 0) { // 'F'
        this._scene.setFlightMode(!this._scene.getFlightMode());
      } else if (event.key === 256 && event.action === 0) { // 'ESC'
        if (this._window.getGameControl()) {
          this._window.setGameControl(false);
        }
      }
    });

    this._window.events.on('mousebutton', event => {
      if (event.button === 0 && event.action === 0) {
        if (!this._window.getGameControl()) {
          this._window.setGameControl(true);
        } else {
          const blockCoord = this._world.findPointingBlock(
            this._camera.getPosition(),
            this._camera.getLook()
          );

          if (blockCoord) {
            this._world.replaceBlock(blockCoord, this._blockIds['block.air']);
          }
        }
      }
    });

    this._scene.events.on('point', coord => {
      console.log(`point event at: ${coord}`)
    })

    const profiler = new misc.Profiler();

    setInterval(() => {
      const metrics = profiler.getMetrics();
      this._textRenderer.write(`${(1000000 / metrics['gameloop_loop']).toFixed(1)} fps`);
      console.log(metrics);
    }, 1000);

    return await new Promise((resolve, reject) => {
      this._gameLoop.start();
      const loop = () => {

        try {
          this._gameLoop.loop();
        } catch (e) {
          reject(e)
          return
        }

        if (!this._window.shouldClose()) {
          setTimeout(loop, 0);
          return;
        }

        this._gameLoop.stop();
        resolve();
      };

      loop();
    });
  }
}

async function createProgram(shaderName) {
  const vertexShader = `${__dirname}/shader/${shaderName}.vert`;
  const fragmentShader = `${__dirname}/shader/${shaderName}.frag`;

  return await gl.Program.create({vertexShader, fragmentShader});
}

/**
 * @param {string} name 
 */
async function createFontLoader(name) {
  const fontLoader = new engine.FontLoader();
  const basePath = `${__dirname}/fonts/${name}`
  const json = JSON.parse(await promisify(fs.readFile)(`${basePath}.json`));
  fontLoader.load(json, `${basePath}.png`);
  return fontLoader;
}