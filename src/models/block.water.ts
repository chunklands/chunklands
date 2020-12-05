import { promisify } from 'util';
import fs from 'fs';
import facesCubeOneTexture from './_geometry/cube-one-texture';
import { ModelLoader } from './types';
const readFile = promisify(fs.readFile);

const blockWater: ModelLoader = async (config) => ({
  type: 'block',
  id: 'block.water',
  faces: facesCubeOneTexture,
  opaque: true,
  texture: await readFile(`${config.assetsDir}/models//block.water.png`),
});

export default blockWater;
