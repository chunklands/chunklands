const game = require('./game_module');
const engine = require('../engine');
const assert = require('assert');
const { EventEmitter } = require('events')
const {promisify} = require('util');
const imageSize = promisify(require('image-size'));
const Jimp = require('jimp');
const { Scene } = require('./game_module');

/**
 * @param {{faces: {[face: string]: number[]}, texture?: string}[]} models 
 */
async function bakeModels(models) {

  // load texture sizes
  const myMetaModels = await Promise.all(
    models.map(async (model, index) => {
      if (!model.texture) {
        return {model, index};
      }

      const size = await imageSize(model.texture);
      return {
        model,
        size,
        index
      };
    })
  );
  
  // sort by size: large textures first is better
  myMetaModels.sort((a, b) => {
    const aSize = a.size ? a.size.width * a.size.height : 0;
    const bSize = b.size ? b.size.width * b.size.height : 0;

    if (aSize < bSize) {
      return 1;
    }

    if (aSize > bSize) {
      return -1;
    }

    return 0;
  });

  const baker = new engine.TextureBaker();
  for (const myMetaModel of myMetaModels) {
    const { size } = myMetaModel;
    console.log({size});
    if (size) {
      myMetaModel.area = baker.addArea(size.width, size.height);
    }
  }

  const textureSize = 1 << getNextPOT(baker.maxDim);
  const texture = await Jimp.create(textureSize, textureSize);

  await Promise.all(myMetaModels.map(async myMetaModel => {
    const { area } = myMetaModel;

    if (!area) {
      return;
    }

    // copy texture
    const modelTexture = await Jimp.create(myMetaModel.model.texture);
    assert(modelTexture.getWidth() === area.w && modelTexture.getHeight() === area.h);
    texture.blit(modelTexture, area.x, area.y);

    // update uv
    const myModel = myMetaModel.model = {...myMetaModel.model};
    const myFaces = myModel.faces = {...myModel.faces};

    for (const faceName in myFaces) {
      const myVertexData = myFaces[faceName] = [...myFaces[faceName]];
      assert(myVertexData.length % 8 === 0);
      console.log({myVertexData})
      for (let i = 0; i < myVertexData.length; i += 8) {
        const uI = i + 6;
        const vI = i + 7;
        
        myVertexData[uI] = (area.x + (myVertexData[uI] * area.w)) / textureSize;
        myVertexData[vI] = (area.y + (myVertexData[vI] * area.h)) / textureSize;
      }
    }
  }));

  myMetaModels.sort((a, b) => a.index < b.index ? 1 : -1);

  return {
    texture,
    models: myMetaModels.map(myMetaModel => myMetaModel.model)
  }
}

function getNextPOT(num) {
  let n;
  for (n = 0; (1 << n) < num; n++) {}

  return n;
}

function createScene() {
  const scene = new Scene();
  scene.events = new EventEmitter();

  return scene;
}

module.exports = {
  ResourceRegistrar,
  ...game,
  createScene,
  bakeModels
};
