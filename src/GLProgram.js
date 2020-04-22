const { GLProgramBase } = require('./module');
const fs = require('fs');
const { promisify } = require('util');

module.exports = class GLProgram extends GLProgramBase {

  static async create({vertexShader, fragmentShader}) {
    const program = new GLProgram();
    await program._compile({vertexShader, fragmentShader});

    return program;
  }

  async _compile({vertexShader, fragmentShader}) {
    const [ vsh, fsh ] = await Promise.all([
      promisify(fs.readFile)(vertexShader),
      promisify(fs.readFile)(fragmentShader)
    ]);

    super._compile(vsh, fsh);
  }
};
