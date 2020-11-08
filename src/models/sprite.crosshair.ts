// clang-format off
import { promisify } from 'util';
import fs from 'fs';
import { ModelLoader } from './types';
const readFile = promisify(fs.readFile);

const spriteCrosshair: ModelLoader = async (config) => ({
  type: 'sprite',
  id: 'sprite.crosshair',
  //  VERTEX1  NORMAL1   UV1   VERTEX2  NORMAL2   UV2   VERTEX3  NORMAL3   UV3
  // prettier-ignore
  data: new Float32Array([
      0, 1, 0, 0, 0, -1, 0, 1, 0, 0, 0, 0, 0, -1, 0, 0, 1, 0, 0, 0, 0, -1, 1, 0,
      0, 1, 0, 0, 0, -1, 0, 1, 1, 0, 0, 0, 0, -1, 1, 0, 1, 1, 0, 0, 0, -1, 1, 1,
  ]).buffer,
  texture: await readFile(`${config.assetsDir}/models/sprite.crosshair.png`),
});

export default spriteCrosshair;
