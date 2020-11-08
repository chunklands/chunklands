import {promisify} from 'util'
import fs from 'fs'
import { readObj } from '../lib/obj';
import { ModelLoader } from './types';
const readFile = promisify(fs.readFile);

const blockMonkey: ModelLoader = async config => {
  const buf = await readObj(fs.createReadStream(`${config.assetsDir}/models/block.monkey.obj`));

  return {
    type: 'block',
    id: 'block.monkey',
    faces: {all: new Float32Array(buf).buffer},
    opaque: true,
    texture: await readFile(`${config.assetsDir}/models/block.monkey.png`)
  };
};

export default blockMonkey