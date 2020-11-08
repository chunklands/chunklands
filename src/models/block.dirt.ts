import {promisify} from 'util'
import fs from 'fs'
import facesCubeOneTexture from './_geometry/cube-one-texture';
import { ModelLoader } from './types';
const readFile = promisify(fs.readFile);

const blockDirt: ModelLoader = async config => ({
  type: 'block',
  id: 'block.dirt',
  faces: facesCubeOneTexture,
  opaque: true,
  texture: await readFile(`${config.assetsDir}/models//block.dirt.png`)
});

export default blockDirt