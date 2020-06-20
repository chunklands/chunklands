const path = require('path');
const { promisify } = require('util');
const globProm = promisify(require('glob'));
const util = require('./util');
const execa = require('execa');

module.exports = class BuildSet {
  constructor({clangfileAbsolutePath, rootAbsolutePath, buildAbsolutePath, debug, clangBin = 'clang', clangTidyBin = 'clang-tidy'}) {
    if (!path.isAbsolute(rootAbsolutePath)) {
      throw new TypeError('need absolute root dir');
    }

    if (!path.isAbsolute(buildAbsolutePath)) {
      throw new TypeError('need absolute build dir');
    }

    if (!path.isAbsolute(clangfileAbsolutePath)) {
      throw new TypeError('need absolute clangfile path');
    }

    this._buildRelativeClangfile = path.relative(buildAbsolutePath, clangfileAbsolutePath);
    this._rootAbsolutePath = rootAbsolutePath;
    this._buildAbsolutePath = buildAbsolutePath;
    this._buildRelativeRootPath = path.relative(buildAbsolutePath, rootAbsolutePath);
    this.debug = debug;

    this.clangBin = clangBin;
    this.clangTidyBin = clangTidyBin;

    this._makefileTargets = {};
  }

  rootRelative(buildRelativeFilePath) {
    const absoluteDir = path.resolve(this._buildAbsolutePath, buildRelativeFilePath);
    const result = path.relative(this._rootAbsolutePath, absoluteDir);
    return result;
  }

  buildRelative(rootRelativePath) {
    // should be normalized
    // we don't like `../build/deps/file.o` but `deps/file.o` for library paths
    const absolutePath = path.join(this._rootAbsolutePath, rootRelativePath);
    return path.relative(this._buildAbsolutePath, absolutePath);
  }

  /**
   * @param {string[]} buildRelativePaths 
   */
  async buildRelativeResolveGlobs(buildRelativePaths) {
    const resolvedDirs = await Promise.all(
      buildRelativePaths.map(p => globProm(p, { cwd: this._buildAbsolutePath }))
    );

    const flatResolved = [];
    for (let i = 0; i < resolvedDirs.length; i++) {
      const resolvedDir = resolvedDirs[i];
      if (resolvedDir.length > 0) {
        flatResolved.push(...resolvedDir);
      } else {
        throw new Error(`warning: globbing ${buildRelativePaths[i]} expands to nothing. This is maybe a typo`);
      }
    }

    return flatResolved;
  }

  clang(args) {
    return execa(this.clangBin, args, { cwd: this._buildAbsolutePath });
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
      cmd: `mkdir -p "${dir}"`
    });

    return this;
  }

  async printMakefile(firstTarget, out = process.stdout) {
    const makefileTargets = this._makefileTargets;
    const buildRelativeClangfile = this._buildRelativeClangfile;

    await util.printMakefile(out, (function *() {
      yield {target: firstTarget, ...makefileTargets[firstTarget]};

      for (const target in makefileTargets) {
        if (target !== firstTarget) {
          yield {
            target,
            ...makefileTargets[target],
            normalDeps: [
              ...makefileTargets[target].normalDeps,
              buildRelativeClangfile
            ]
          };
        }
      }
    })());
  }
}