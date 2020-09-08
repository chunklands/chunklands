const fs = require('fs');
const readObj = require('../../lib/obj');
const {promisify} = require('util');
const readFile = promisify(fs.readFile);

module.exports = async () => {
  const buf = await readObj(fs.createReadStream(`${__dirname}/pickaxe.obj`));

  return {
    id: 'block.pickaxe',
    faces: {all: new Float32Array(buf).buffer},
    opaque: true,
    texture: await readFile(`${__dirname}/pickaxe.png`)
  };
};