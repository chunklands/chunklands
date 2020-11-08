import {promisify} from 'util'
import fs from 'fs'
import { readObj } from '../lib/obj';
import { ModelLoader } from './types';
const readFile = promisify(fs.readFile);

const blockPickaxe: ModelLoader = async config => {
  const buf = await readObj(fs.createReadStream(`${config.assetsDir}/models/block.pickaxe.obj`));

  return {
    type: 'block',
    id: 'block.pickaxe',
    faces: {all: new Float32Array(buf).buffer},
    opaque: true,
    texture: await readFile(`${config.assetsDir}/models/block.pickaxe.png`)
  };
};

export default blockPickaxe