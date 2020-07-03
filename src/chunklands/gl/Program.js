const fs = require('fs');
const { promisify } = require('util');
const { ProgramBase } = require('./_');

module.exports = class Program extends ProgramBase {

  static async create({vertexShader, fragmentShader}, opts) {
    const program = new Program();
    await program.compile({vertexShader, fragmentShader}, opts);

    return program;
  }

  async compile({vertexShader, fragmentShader}, {defines = {}} = {}) {
    const [ vsh, fsh ] = await Promise.all([
      promisify(fs.readFile)(vertexShader),
      promisify(fs.readFile)(fragmentShader)
    ]);

    super.compile(
      shaderAddDefines(vsh.toString(), defines),
      shaderAddDefines(fsh.toString(), defines)
    );
  }
};

/**
 * 
 * @param {string} code 
 * @param {{[key: string]: any}} defines 
 */
function shaderAddDefines(code, defines) {
  if (!/(^#version\s.+)\n/m.test(code)) {
    throw new TypeError(`shader doesn't start with #version`);
  }

  const definesPrefix = [];
  for (const key in defines) {
    const value = defines[key];
    if (typeof value === 'boolean') {
      if (value) {
        definesPrefix.push(`#define ${key}`);
      }
    } else {
      definesPrefix.push(`#define ${key} ${value}`);
    }
  }

  const prefix = definesPrefix.join('\n');

  return `${RegExp.$1}\n\n${prefix}\n\n${code.substr(RegExp.$1.length + 1)}`;
}