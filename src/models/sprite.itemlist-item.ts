// clang-format off
import {promisify} from 'util'
import fs from 'fs'
import { ModelLoader } from './types';
import { tuv } from './sprite.itemlist.util';
const readFile = promisify(fs.readFile);

const dx = 1 / 128;

const spriteItemlistItem: ModelLoader = async config => ({
  type: 'sprite',
  id: 'sprite.itemlist-item',
  data: new Float32Array(tuv([
    0, 1, 0, 0, 0, -1, dx, 1-dx,
    0, 0, 0, 0, 0, -1, dx, dx,
    1, 0, 0, 0, 0, -1, 1-dx, dx,

    0, 1, 0, 0, 0, -1, dx, 1-dx,
    1, 0, 0, 0, 0, -1, 1-dx, dx,
    1, 1, 0, 0, 0, -1, 1-dx, 1-dx,
  ])).buffer,
  texture: await readFile(`${config.assetsDir}/models/sprite.itemlist-item.png`)
});

export default spriteItemlistItem