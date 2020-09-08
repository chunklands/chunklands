// clang-format off
const {promisify} = require('util');
const readFile = promisify(require('fs').readFile);

const dx = 1 / 128;

function tuv(arr) {
  const result = [...arr];
  for (let v = 7; v < result.length; v += 8) {
    result[v] = 1 - result[v]
  }

  return result;
}

module.exports = async () => ({
  id: 'sprite.itemlist-item',
  data: new Float32Array(tuv([
    0, 1, 0, 0, 0, -1, dx, 1-dx,
    0, 0, 0, 0, 0, -1, dx, dx,
    1, 0, 0, 0, 0, -1, 1-dx, dx,

    0, 1, 0, 0, 0, -1, dx, 1-dx,
    1, 0, 0, 0, 0, -1, 1-dx, dx,
    1, 1, 0, 0, 0, -1, 1-dx, 1-dx,
  ])).buffer,
  texture: await readFile(`${__dirname}/item.png`)
});