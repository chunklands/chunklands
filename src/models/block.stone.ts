import {promisify} from 'util'
import fs from 'fs'
import facesCubeOneTexture from './_geometry/cube-one-texture';
import { ModelLoader } from './types';
const readFile = promisify(fs.readFile);

const blockStone: ModelLoader = async config => ({
  type: 'block',
  id: 'block.stone',
  faces: facesCubeOneTexture,
  opaque: true,
  texture: await readFile(`${config.assetsDir}/models/block.stone.png`)
});

export default blockStone