const { engine: { Environment, createWindow }, misc: { Profiler } } = require('./chunklands');
const Game = require('./game/Game');

Environment.initialize();

const window = createWindow();
window.initialize({
  width: 1280,
  height: 720,
  title: 'Chunklands'
});

// set opengl context
window.makeContextCurrent();
Environment.loadProcs();

const game = new Game(window);

let profilerInterval;

(async () => {
  await game.initialize();

  const profiler = new Profiler();

  profilerInterval = setInterval(() => {
    console.log(profiler.getMetrics());
  }, 1000);

  await game.run();
})()
  .finally(() => {
    clearInterval(profilerInterval);
    window.close()
    Environment.terminate();
  })
  .then(() => {
    process.exit(0);
  })
  .catch(e => {
    console.error(e);
    process.exit(1);
  });
