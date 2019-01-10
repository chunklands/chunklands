const Environment = require('./Environment');
const GameLoop = require('./GameLoop');
const Scene = require('./Scene');
const Window = require('./Window');

Environment.initialize();

const window = new Window({
  width: 640,
  height: 480,
  title: 'Chunklands'
});

// set opengl context
window.makeContextCurrent();
Environment.loadProcs();

const scene = new Scene();
scene.setWindow(window);

const gameLoop = new GameLoop();
gameLoop.setScene(scene);

gameLoop.start();

const loop = () => {
  // in order to have more accurate loop, immediately set timeout
  const timeoutId = setTimeout(loop, 1000 / 66 /*Hz*/);
  gameLoop.loop();
  
  if (window.shouldClose) {
    clearTimeout(timeoutId);
    window.close();
    gameLoop.stop();
    Environment.terminate();
    return;
  }
};

loop();