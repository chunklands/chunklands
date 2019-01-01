const {initialize, pollEvents, terminate} = require('./module');
const Loop = require('./Loop');
const Window = require('./Window');

initialize();

const loop = new Loop(60);

const window = new Window({
  width: 640,
  height: 480,
  title: 'Chunklands'
});

window.makeContextCurrent();
window.on('key', data => {
  console.log(data);
});

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
  window.swapBuffers();
});