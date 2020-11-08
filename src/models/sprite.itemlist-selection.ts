import { tuv } from "./sprite.itemlist.util";
import { ModelLoader } from "./types";

// clang-format off
const {promisify} = require('util');
const readFile = promisify(require('fs').readFile);

const dx = 1 / 128;

const spriteItemlistSelection: ModelLoader = async config => ({
  type: 'sprite',
  id: 'sprite.itemlist-selection',
  data: new Float32Array(tuv([
    0, 1, 0, 0, 0, -1, dx, 1-dx,
    0, 0, 0, 0, 0, -1, dx, dx,
    1, 0, 0, 0, 0, -1, 1-dx, dx,

    0, 1, 0, 0, 0, -1, dx, 1-dx,
    1, 0, 0, 0, 0, -1, 1-dx, dx,
    1, 1, 0, 0, 0, -1, 1-dx, 1-dx,
  ])).buffer,
  texture: await readFile(`${config.assetsDir}/models/sprite.itemlist-selection.png`)
});

export default spriteItemlistSelection