const {initialize, pollEvents, terminate, loadProcs} = require('./module');
const Loop = require('./Loop');
const Window = require('./Window');
const Scene = require('./Scene');

initialize();

const loop = new Loop(60);

const window = new Window({
  width: 640,
  height: 480,
  title: 'Chunklands'
});

const scene = new Scene();
window.makeContextCurrent();

loadProcs();
scene.prepare(window);

loop.start();
loop.on('loop', diff => {

  pollEvents();

  if (window.shouldClose) {
    window.close();
    loop.stop();
    terminate();
    console.log('terminate');
    return;
  }

  window.clear();
  scene.render(diff);
  
  window.swapBuffers();
});