const path = require('path');
const { promisify } = require('util');
const globProm = promisify(require('glob'));
const util = require('./util');
const execa = require('execa');

module.exports = class BuildSet {
  constructor({clangfileAbsolutePath, rootAbsolutePath, buildAbsolutePath, debug, clangBin = 'clang', clangTidyBin = 'clang-tidy', clangNoTidy = false}) {
    if (!path.isAbsolute(rootAbsolutePath)) {
      throw new TypeError('need absolute root dir');
    }

    if (!path.isAbsolute(buildAbsolutePath)) {
      throw new TypeError('need absolute build dir');
    }

    if (!path.isAbsolute(clangfileAbsolutePath)) {
      throw new TypeError('need absolute clangfile path');
    }

    this._clangfileAbsolutePath = clangfileAbsolutePath;
    this._rootAbsolutePath = rootAbsolutePath;
    this._buildAbsolutePath = buildAbsolutePath;
    this.debug = debug;

    this.clangBin = clangBin;
    this.clangTidyBin = clangTidyBin;
    this.clangNoTidy = clangNoTidy;

    this._makefileTargets = {};
  }

  rootRelative(buildRelativeFilePath) {
    const absoluteDir = path.resolve(this._buildAbsolutePath, buildRelativeFilePath);
    const result = path.relative(this._rootAbsolutePath, absoluteDir);
    return result;
  }

  /**
   * @param {string[]} paths 
   */
  async resolveGlobs(paths) {
    const resolvedDirs = await Promise.all(
      paths.map(p => globProm(p, { cwd: this._rootAbsolutePath }))
    );

    const flatResolved = [];
    for (let i = 0; i < resolvedDirs.length; i++) {
      const resolvedDir = resolvedDirs[i];
      if (resolvedDir.length > 0) {
        flatResolved.push(...resolvedDir);
      } else {
        throw new Error(`warning: globbing ${paths[i]} expands to nothing. This is maybe a typo`);
      }
    }

    return flatResolved;
  }

  clang(args) {
    return execa(this.clangBin, args, { cwd: this._rootAbsolutePath });
  }

  addMakefileTarget(target, {normalDeps = [], orderOnlyDeps = [], cmd}) {
    if (target in this._makefileTargets) {
      throw new TypeError(`target ${target} already exists in Makefile`);
    }

    const dir = path.dirname(target);

    this._makefileTargets[target] = {
      normalDeps,
      orderOnlyDeps: [...orderOnlyDeps, dir],
      cmd
    };

    
    if (dir === '' || dir === '.' || dir in this._makefileTargets) {
      return;
    }

    this.addMakefileTarget(dir, {
      cmd: `@mkdir -p "${dir}"`
    });

    return this;
  }

  async printMakefile(firstTarget, out = process.stdout) {
    const makefileTargets = this._makefileTargets;
    const clangfilePath = path.relative(this._rootAbsolutePath, this._clangfileAbsolutePath);

    await util.printMakefile(out, (function *() {
      yield {target: firstTarget, ...makefileTargets[firstTarget]};

      for (const target in makefileTargets) {
        if (target !== firstTarget) {
          yield {
            target,
            ...makefileTargets[target],
            normalDeps: [
              ...makefileTargets[target].normalDeps,
              clangfilePath
            ]
          };
        }
      }
    })());
  }
}