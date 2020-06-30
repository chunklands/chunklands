const fs = require('fs');
const { promisify } = require('util');
const { ProgramBase } = require('./_');

module.exports = class Program extends ProgramBase {

  static async create({vertexShader, fragmentShader}) {
    const program = new Program();
    await program.compile({vertexShader, fragmentShader});

    return program;
  }

  async compile({vertexShader, fragmentShader}) {
    const [ vsh, fsh ] = await Promise.all([
      promisify(fs.readFile)(vertexShader),
      promisify(fs.readFile)(fragmentShader)
    ]);

    super.compile(vsh, fsh);
  }
};