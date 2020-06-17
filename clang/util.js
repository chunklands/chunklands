const execa = require('execa');
const path = require('path');

module.exports = {
  /**
   * @param {string[]} args
   */
  clang(args, {cwd}) {
    return execa('clang', args, { cwd });
  },
  sourceToObjectPath(source) {
    const parsed = path.parse(source);
    return path.join(parsed.dir, parsed.name + '.o');
  },
  async printMakefile(out, entries) {
    for (const entry of entries) {
      await new Promise((resolve, reject) => {
        
        const normalDeps = [...(entry.normalDeps ?? [])].join(' \\\n  ');
        const orderOnlyDeps = (entry.orderOnlyDeps ?? []).join(' \\\n  ');
        out.write(`${entry.target}: ${normalDeps} | ${orderOnlyDeps}\n\t${entry.cmd || ''}\n\n`, err => {
          if (err) {
            reject(err);
            return;
          }

          resolve();
        })
      });
    }
  }
}