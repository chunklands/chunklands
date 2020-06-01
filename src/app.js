const { engine: { Environment, createWindow } } = require('./chunklands');
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

(async () => {
  await game.initialize();
  await game.run();
})()
  .finally(() => {
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
