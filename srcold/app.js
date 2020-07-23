const { game: { DefaultGame } } = require('./chunklands');

const game = new DefaultGame();

game.run()
  .then(() => process.exit(0))
  .catch(e => {
    console.error(e);
    process.exit(1);
  });
