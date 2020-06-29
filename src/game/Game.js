const fs = require('fs');
const { promisify } = require('util');
const {
  engine: {
    GBufferPass,
    SSAOPass,
    SSAOBlurPass,
    LightingPass,
    SkyboxPass,
    MovementController,
    Skybox,
    GameLoop,
    Camera,
    Window: EngineWindow,
    FontLoader,
    TextRenderer,
    BlockSelectPass,
  },
  game: {
    ChunkGenerator,
    World,
    createScene,
    bakeModels
  },
  gl: {
    Program
  },
  misc: {
    Profiler
  }
} = require('../chunklands');
const SimpleWorldGen = require('./world/SimpleWorldGen')
const modelLoader    = require('./models');
const game = require('../chunklands/modules/game');
const files = require('../files');
const { GameOverlayRenderer } = require('../chunklands/modules/engine');

module.exports = class Game {
  /**
   * 
   * @param {EngineWindow} window 
   */
  constructor(window) {
    this._window = window;
  }

  async initialize() {


    // blocks
    const models = await modelLoader();
    const bakedModels = await bakeModels(models);

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

    const chunkGenerator = new ChunkGenerator();
    chunkGenerator.setBlockRegistrar(blockRegistrar);

    
    // world

    const worldGenerator = new SimpleWorldGen(this._blockIds);
    chunkGenerator.setWorldGenerator(worldGenerator);

    this._world = new World();
    this._world.setChunkGenerator(chunkGenerator);
    this._world.setBlockRegistrar(blockRegistrar);

    // game overlay
    const gameOverlay = new game.GameOverlay();
    gameOverlay.setSpriteRegistrar(spriteRegistrar);

    // scene
    const scene = createScene();
    scene.setGameOverlay(gameOverlay);
    scene.setBlockRegistrar(blockRegistrar);
    scene.setWorld(this._world);


    // render pipeline
    {
      const gbufferPass = new GBufferPass();
      gbufferPass.setProgram(await createProgram('gbuffer'));
      scene.setGBufferPass(gbufferPass);
    }

    {
      const ssaoPass = new SSAOPass();
      ssaoPass.setProgram(await createProgram('ssao'));
      scene.setSSAOPass(ssaoPass);
    }

    {
      const ssaoBlurPass = new SSAOBlurPass();
      ssaoBlurPass.setProgram(await createProgram('ssaoblur'));
      scene.setSSAOBlurPass(ssaoBlurPass);
    }

    {
      const lightingPass = new LightingPass();
      lightingPass.setProgram(await createProgram('lighting'));
      scene.setLightingPass(lightingPass);
    }

    {
      const skyboxPass = new SkyboxPass();
      skyboxPass.setProgram(await createProgram('skybox'));
      scene.setSkyboxPass(skyboxPass);
    }


    // skybox
    {
      const skybox = new Skybox();
      skybox.initialize(`${__dirname}/skyboxes/skybox_day/`);
      scene.setSkybox(skybox);
    }

    // game overlay
    {
      const gameOverlayRenderer = new GameOverlayRenderer();
      gameOverlayRenderer.setProgram(await createProgram('game_overlay'));
      scene.setGameOverlayRenderer(gameOverlayRenderer);
    }

    // block select
    {
      const blockSelectPass = new BlockSelectPass();
      blockSelectPass.setProgram(await createProgram('block_select'));
      scene.setBlockSelectPass(blockSelectPass);
    }


    // camera
    this._camera = new Camera();
    this._camera.setPosition(0, 30, 0);
    scene.setCamera(this._camera);


    // movement controller
    const movementController = new MovementController();
    movementController.setCamera(this._camera);
    movementController.setCollisionSystem(this._world);
    scene.setMovementController(movementController);

    // text renderer
    const fontLoader = await createFontLoader('ubuntu');
    const textRenderer = new TextRenderer();
    textRenderer.setFontLoader(fontLoader);
    textRenderer.setProgram(await createProgram('text'));
    scene.setTextRenderer(textRenderer);


    // TODO(daaitch): add preparation phase: this has to be at the end to update buffers
    scene.setWindow(this._window);

    // gameloop

    const gameLoop = new GameLoop();
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

    const profiler = new Profiler();

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

  return await Program.create({vertexShader, fragmentShader});
}

/**
 * @param {string} name 
 */
async function createFontLoader(name) {
  const fontLoader = new FontLoader();
  const basePath = `${__dirname}/fonts/${name}`
  const json = JSON.parse(await promisify(fs.readFile)(`${basePath}.json`));
  fontLoader.load(json, `${basePath}.png`);
  return fontLoader;
}