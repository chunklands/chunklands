const fs = require('fs');
const readObj = require('../../lib/obj');
const { promisify } = require('util');
const readFile = promisify(fs.readFile);

module.exports = async () => {

  const buf = await readObj(fs.createReadStream(`${__dirname}/monkey.obj`));

  return {
    id: 'block.monkey',
    faces: {
      all: buf
    },
    opaque: true,
    texture: await readFile(`${__dirname}/gold.png`)
  };
};