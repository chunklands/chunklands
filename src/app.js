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

const chunkGenerator = new ChunkGenerator();

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