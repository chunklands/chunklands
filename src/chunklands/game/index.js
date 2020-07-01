const { EventEmitter } = require('events');
const game = require('./_');
const bakeModels = require('./bakeModels');

function createScene() {
  const scene = new game.Scene();
  scene.events = new EventEmitter();

  return scene;
}

module.exports = {
  ...game,
  createScene,
  bakeModels
};
