import { promisify } from 'util';
import fs from 'fs';
import { ModelLoader } from './types';
const readFile = promisify(fs.readFile);

const dx = 1 / 128;

const blockGrass: ModelLoader = async (config) => ({
  type: 'block',
  id: 'block.grass',
  // prettier-ignore
  faces: {
      // VERTEX1      NORMAL1          UV1           VERTEX2         NORMAL2          UV2             VERTEX3         NORMAL3          UV3
    front: new Float32Array([
      0.0, 1.0, 0.0,  0.0, 0.0, -1.0,  dx,  0.5+dx,  0.0, 0.0, 0.0,  0.0, 0.0, -1.0,  dx,     1.0-dx, 1.0, 0.0, 0.0,  0.0, 0.0, -1.0,  0.5-dx, 1.0-dx,
      0.0, 1.0, 0.0,  0.0, 0.0, -1.0,  dx,  0.5+dx,  1.0, 0.0, 0.0,  0.0, 0.0, -1.0,  0.5-dx, 1.0-dx, 1.0, 1.0, 0.0,  0.0, 0.0, -1.0,  0.5-dx, 0.5+dx,
    ]).buffer, 
    back: new Float32Array([ 
      1.0, 1.0, 1.0,  0.0, 0.0, +1.0,  dx,  0.5+dx,  1.0, 0.0, 1.0,  0.0, 0.0, +1.0,  dx,     1.0-dx, 0.0, 0.0, 1.0,  0.0, 0.0, +1.0,  0.5-dx, 1.0-dx,
      1.0, 1.0, 1.0,  0.0, 0.0, +1.0,  dx,  0.5+dx,  0.0, 0.0, 1.0,  0.0, 0.0, +1.0,  0.5-dx, 1.0-dx, 0.0, 1.0, 1.0,  0.0, 0.0, +1.0,  0.5-dx, 0.5+dx,
    ]).buffer, 
    left: new Float32Array([ 
      0.0, 1.0, 1.0,  -1.0, 0.0, 0.0,  dx,  0.5+dx,  0.0, 0.0, 1.0,  -1.0, 0.0, 0.0,  dx,     1.0-dx, 0.0, 0.0, 0.0,  -1.0, 0.0, 0.0,  0.5-dx, 1.0-dx,
      0.0, 1.0, 1.0,  -1.0, 0.0, 0.0,  dx,  0.5+dx,  0.0, 0.0, 0.0,  -1.0, 0.0, 0.0,  0.5-dx, 1.0-dx, 0.0, 1.0, 0.0,  -1.0, 0.0, 0.0,  0.5-dx, 0.5+dx,
    ]).buffer, 
    right: new Float32Array([ 
      1.0, 1.0, 0.0,  +1.0, 0.0, 0.0,  dx,  0.5+dx,  1.0, 0.0, 0.0,  +1.0, 0.0, 0.0,  dx,     1.0-dx, 1.0, 0.0, 1.0,  +1.0, 0.0, 0.0,  0.5-dx, 1.0-dx,
      1.0, 1.0, 0.0,  +1.0, 0.0, 0.0,  dx,  0.5+dx,  1.0, 0.0, 1.0,  +1.0, 0.0, 0.0,  0.5-dx, 1.0-dx, 1.0, 1.0, 1.0,  +1.0, 0.0, 0.0,  0.5-dx, 0.5+dx,
    ]).buffer, 
    top: new Float32Array([ 
      0.0, 1.0, 1.0,  0.0, +1.0, 0.0,  dx,  dx,   0.0, 1.0, 0.0,  0.0, +1.0, 0.0,     dx,     0.5-dx, 1.0, 1.0, 0.0,  0.0, +1.0, 0.0,  0.5-dx, 0.5-dx,
      0.0, 1.0, 1.0,  0.0, +1.0, 0.0,  dx,  dx,   1.0, 1.0, 0.0,  0.0, +1.0, 0.0,     0.5-dx, 0.5-dx, 1.0, 1.0, 1.0,  0.0, +1.0, 0.0,  0.5-dx, dx,
    ]).buffer, 
    bottom: new Float32Array([ 
      0.0, 0.0, 0.0,  0.0, -1.0, 0.0,  0.5+dx, dx,   0.0, 0.0, 1.0,  0.0, -1.0, 0.0,  0.5+dx, 0.5-dx, 1.0, 0.0, 1.0,  0.0, -1.0, 0.0,  1.0-dx, 0.5-dx,
      0.0, 0.0, 0.0,  0.0, -1.0, 0.0,  0.5+dx, dx,   1.0, 0.0, 1.0,  0.0, -1.0, 0.0,  1.0-dx, 0.5-dx, 1.0, 0.0, 0.0,  0.0, -1.0, 0.0,  1.0-dx, dx,
    ]).buffer,
  },
  opaque: true,
  texture: await readFile(`${config.assetsDir}/models/block.grass.png`),
});

export default blockGrass;
