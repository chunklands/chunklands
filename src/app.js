const {initialize, terminate, loadProcs} = require('./module');
const GameLoop = require('./GameLoop');
const Window = require('./Window');
const Scene = require('./Scene');

initialize();

const window = new Window({
  width: 640,
  height: 480,
  title: 'Chunklands'
});

// set opengl context
window.makeContextCurrent();
loadProcs();

const scene = new Scene();
scene.setWindow(window);

const gameLoop = new GameLoop();
gameLoop.setScene(scene);

gameLoop.start();

const loop = () => {
  gameLoop.loop();
  
  if (window.shouldClose) {
    window.close();
    gameLoop.stop();
    terminate();
    return;
  }

  setTimeout(loop, 1000 / 66 /*Hz*/);
};

loop();