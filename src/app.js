const BlockRegistrar = require('./BlockRegistrar');
const ChunkGenerator = require('./ChunkGenerator');
const Environment = require('./Environment');
const GameLoop = require('./GameLoop');
const Scene = require('./Scene');
const Window = require('./Window');
const World = require('./World');

Environment.initialize();

const window = new Window({
  width: 640,
  height: 480,
  title: 'Chunklands'
});

// set opengl context
window.makeContextCurrent();
Environment.loadProcs();

const blockRegistrar = new BlockRegistrar();
blockRegistrar.addBlock(require('./game/blocks/air'));
blockRegistrar.addBlock(require('./game/blocks/block'));

const chunkGenerator = new ChunkGenerator();
chunkGenerator.setBlockRegistrar(blockRegistrar);

const world = new World();
world.setChunkGenerator(chunkGenerator);

const scene = new Scene();
scene.setWindow(window);
scene.setWorld(world);

const gameLoop = new GameLoop();
gameLoop.setScene(scene);

gameLoop.start();

const loop = () => {
  gameLoop.loop();
  
  if (window.shouldClose) {
    window.close();
    gameLoop.stop();
    Environment.terminate();
    return;
  }

  setImmediate(loop);
};

loop();