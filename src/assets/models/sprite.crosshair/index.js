// clang-format off
const {promisify} = require('util');
const readFile = promisify(require('fs').readFile);
const x = 1 / 64;

module.exports = async () => ({
  id: 'sprite.crosshair',
  //  VERTEX1              NORMAL1   UV1   VERTEX2              NORMAL2   UV2   VERTEX3              NORMAL3   UV3
  data: new Float32Array([
      0.5 - x, 0.5 + x, 0, 0, 0, -1, 0, 1, 0.5 - x, 0.5 - x, 0, 0, 0, -1, 0, 0, 0.5 + x, 0.5 - x, 0, 0, 0, -1, 1, 0,
      0.5 - x, 0.5 + x, 0, 0, 0, -1, 0, 1, 0.5 + x, 0.5 - x, 0, 0, 0, -1, 1, 0, 0.5 + x, 0.5 + x, 0, 0, 0, -1, 1, 1,
  ]).buffer,
  texture: await readFile(`${__dirname}/crosshair.png`)
});