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
    Window: EngineWindow
  },
  game: {
    BlockRegistrar,
    ChunkGenerator,
    World,
    Scene
  },
  gl: {
    Program
  }
} = require('../chunklands');
const SimpleWorldGen = require('./world/SimpleWorldGen')
const blocks     = require('./blocks');

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

    const blockRegistrar = new BlockRegistrar();
    for (const block of blocks) {
      blockRegistrar.addBlock(block);
    }
    await blockRegistrar.bake();

    const chunkGenerator = new ChunkGenerator();
    chunkGenerator.setBlockRegistrar(blockRegistrar);

    
    // world

    const worldGenerator = new SimpleWorldGen(blockRegistrar.getBlockIds());
    chunkGenerator.setWorldGenerator(worldGenerator);

    const world = new World();
    world.setChunkGenerator(chunkGenerator);


    // scene

    const scene = new Scene();
    scene.setWorld(world);


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
      skybox.initialize(`${__dirname}/skyboxes/skyboxsun5deg2/`);
      scene.setSkybox(skybox);
    }


    // camera
    const camera = new Camera();
    camera.setPosition(0, 10, 0);
    scene.setCamera(camera);


    // movement controller
    const movementController = new MovementController();
    movementController.setCamera(camera);
    movementController.setCollisionSystem(world);
    scene.setMovementController(movementController);


    // TODO(daaitch): add preparation phase: this has to be at the end to update buffers
    scene.setWindow(this._window);


    // gameloop

    const gameLoop = new GameLoop();
    gameLoop.setScene(scene);

    this._gameLoop = gameLoop;
  }

  async run() {
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
  const vertexShader = `${__dirname}/shader/${shaderName}.vsh.glsl`;
  const fragmentShader = `${__dirname}/shader/${shaderName}.fsh.glsl`;

  return await Program.create({vertexShader, fragmentShader});
}