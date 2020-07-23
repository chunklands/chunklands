const { EventEmitter } = require('events');
const game = require('./_');
const bakeModels = require('./bakeModels');
const DefaultGame = require('./DefaultGame');
const ModelLoader = require('./ModelLoader');
const RenderPipeline = require('./RenderPipeline');
const SimpleWorldGen = require('./SimpleWorldGen');

function createScene() {
  const scene = new game.Scene();
  scene.events = new EventEmitter();

  return scene;
}

module.exports = {
  ...game,
  createScene,
  bakeModels,
  DefaultGame,
  ModelLoader,
  RenderPipeline,
  SimpleWorldGen,
};
