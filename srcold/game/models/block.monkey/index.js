const fs = require('fs');
const readObj = require('../../lib/obj');

module.exports = async () => {

  const buf = await readObj(fs.createReadStream(`${__dirname}/monkey.obj`));

  return {
    id: 'block.monkey',
    faces: {
      all: buf
    },
    opaque: true,
    texture: `${__dirname}/gold.png`
  };
};