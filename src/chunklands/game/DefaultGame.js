const { EventEmitter } = require('events');
const engine = require('../engine');
const game = require('./_');
const ModelLoader = require('./ModelLoader');
const modelLoader = require('../../game/models');
const SimpleWorldGen = require('./SimpleWorldGen');
const files = require('../../files');
const RenderPipeline = require('./RenderPipeline');
const misc = require('../misc');

class DefaultGame {
  constructor() {

  }

  async run() {
    engine.Environment.initialize();

    this._window = new engine.Window();
    this._window.events = new EventEmitter();
    this._window.initialize({title: 'Chunklands', width: 1024, height: 768});
    this._window.makeContextCurrent();
    engine.Environment.loadProcs();

    this._modelLoader = new ModelLoader();
    await this._modelLoader.load(modelLoader, files.cacheFile('texture.png'))

    // ====> HERE

    this._chunkGenerator = new game.ChunkGenerator();
    this._chunkGenerator.setBlockRegistrar(this._modelLoader.blockRegistrar);

    this._worldGenerator = new SimpleWorldGen(this._modelLoader.blockIds);
    this._chunkGenerator.setWorldGenerator(this._worldGenerator);

    this._world = new game.World();
    this._world.setChunkGenerator(this._chunkGenerator);
    this._world.setBlockRegistrar(this._modelLoader.blockRegistrar);

    this._gameOverlay = new game.GameOverlay();
    this._gameOverlay.setSpriteRegistrar(this._modelLoader.spriteRegistrar);

    this._scene = new game.Scene();
    this._scene.events = new EventEmitter();
    this._scene.setGameOverlay(this._gameOverlay);
    this._scene.setModelTexture(this._modelLoader.texture);
    this._scene.setWorld(this._world);

    const ssao = true;
    this._scene.setSSAO(ssao);

    this._renderPipeline = new RenderPipeline();
    await this._renderPipeline.initialize({
      scene: this._scene,
      font: 'ubuntu',
      ssao
    });

    this._camera = new engine.Camera();
    this._camera.setPosition(0, 30, 0);
    this._scene.setCamera(this._camera);

    this._movementController = new engine.MovementController();
    this._movementController.setCamera(this._camera);
    this._movementController.setCollisionSystem(this._world);
    this._scene.setMovementController(this._movementController);

    // TODO(daaitch): triggers preparation, should be another scene.prepare()
    this._scene.setWindow(this._window);

    this._gameLoop = new engine.GameLoop();
    this._gameLoop.setScene(this._scene);

    this._registerListener();
    this._setupProfiler();

    this._gameLoop.start();

    try {
      await new Promise((resolve, reject) => {
        const loop = () => {
          try {
            this._gameLoop.loop();
          } catch (e) {
            reject(e);
            return;
          }

          if (!this._window.shouldClose()) {
            setTimeout(loop, 0);
            return;
          }

          resolve();
        };

        loop();
      });
    } finally {
      this._gameLoop.stop();
      this._window.close();

      engine.Environment.terminate();
    }
  }

  _setupProfiler() {
    const profiler = new misc.Profiler();
    setInterval(() => {
      const metrics = profiler.getMetrics();
      this._renderPipeline._textRenderer.write(`${(1000000 / metrics['gameloop_loop']).toFixed(1)} fps`);
      // console.log(metrics);
    }, 1000);
  }

  _registerListener() {
    // flight-mode
    this._window.events.on('key', event => {
      if (event.key === 70 && event.action === 0) { // 'F'
        this._scene.setFlightMode(!this._scene.getFlightMode());
      } else if (event.key === 256 && event.action === 0) { // 'ESC'
        if (this._window.getGameControl()) {
          this._window.setGameControl(false);
        }
      } else if (event.key === 32 && event.action === 1) { // Space
        if (!this._scene.getFlightMode() && this._window.getGameControl()) {
          console.log('JUMP')
          this._scene.jump();
        }
      }
    });

    // game control
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
            this._world.replaceBlock(blockCoord, this._modelLoader.blockIds['block.air']);
          }
        }
      }
    });

    // point event
    this._scene.events.on('point', coord => {
      console.log(`point event at: ${coord}`)
    })
  }
}

module.exports = DefaultGame;