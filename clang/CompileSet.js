
const path = require('path');
const util = require('./util');

module.exports = class CompileSet {
  /**
   * 
   * @param {import('./BuildSet')} buildSet 
   * @param {*} param1 
   */
  constructor(buildSet, {std, fPIC, shared}) {
    this._buildRelativeIncludeDirGlobs = [];
    this._buildRelativeSourceGlobs = [];

    this._buildRelativeIncludeDirs = [];
    this._buildRelativeSources = [];
    this._buildRelativeLibraries = [];
    this._buildLink = [];

    this._options = {
      std,
      fPIC,
      shared
    };
    this._buildSet = buildSet;
  }

  addInclude(...dirs) {
    for (const dir of dirs) {
      this._buildRelativeIncludeDirGlobs.push(this._buildSet.buildRelative(dir))
    }

    return this;
  }

  addSource(...files) {
    for (const file of files) {
      this._buildRelativeSourceGlobs.push(this._buildSet.buildRelative(file))
    }

    return this;
  }

  addLibrary(...files) {
    for (const file of files) {
      this._buildRelativeLibraries.push(this._buildSet.buildRelative(file));
    }
    console.log({x: this._buildRelativeLibraries})

    return this;
  }

  addLink(...libs) {
    this._buildLink.push(...libs);
    return this;
  }

  async _prepare() {
    [
      this._buildRelativeSources,
      this._buildRelativeIncludeDirs,
    ] = await Promise.all([
      this._buildSet.buildRelativeResolveGlobs(this._buildRelativeSourceGlobs),
      this._buildSet.buildRelativeResolveGlobs(this._buildRelativeIncludeDirGlobs),
    ]);
  }

  _clangIncludeDirsArgs() {
    const args = [];
    for (const includeDir of this._buildRelativeIncludeDirs) {
      args.push('-I', includeDir);
    }

    return args;
  }

  _clangLinkArgs() {
    return this._buildLink.map(lib => `-l${lib}`);
  }

  _clangInputArgs() {
    return this._buildRelativeSources;
  }

  _makefileProgDependencies() {
    const deps = []
    for (const source of this._buildRelativeSources) {
      deps.push(this._buildSet.rootRelativeAbsolutePath(util.sourceToObjectPath(source)));
    }

    return deps;
  }

  _clangObjectCmd(source) {
    return [
      'clang',
      '-c',
      this._options.fPIC ? '-fPIC' : null,
      this._options.std ? `-std=${this._options.std}` : null,
      ...this._clangIncludeDirsArgs(),
      source,
      '-o',
      util.sourceToObjectPath(this._buildSet.rootRelativeAbsolutePath(source))
    ].filter(x => x).join(' ');
  }

  _clangCompileCmd(target, inputs) {
    return [
      'clang',
      this._options.shared ? '-shared' : null,
      this._options.fPIC ? '-fPIC' : null,
      this._options.std ? `-std=${this._options.std}` : null,
      ...this._clangIncludeDirsArgs(),
      ...inputs,
      ...this._buildRelativeLibraries,
      ...this._clangLinkArgs(),
      '-o',
      target
    ].filter(x => x).join(' ');
  }

  async addToBuildSet(resultTarget) {
    await this._prepare();

    const includeArgs = this._clangIncludeDirsArgs();

    const targets = [];

    for (const source of this._buildRelativeSources) {
      const targetWithDeps = await this._makefileTarget(source, includeArgs);
      const [target, depsString] = targetWithDeps.split(':', 2);
      const deps = depsString.trim().replace(/\s*\\\n\s*/gm, ' ').split(' ');
      this._buildSet.addMakefileTarget(target, {
        normalDeps: deps,
        cmd: this._clangObjectCmd(source)
      });

      targets.push(target);
    }

    if (resultTarget) {
      this._buildSet.addMakefileTarget(resultTarget, {
        normalDeps: [
          ...this._makefileProgDependencies(),
          ...this._buildRelativeLibraries
        ],
        cmd: this._clangCompileCmd(resultTarget, targets)
      });
    }
  }

  async _makefileTarget(filename, includeArgs) {
    const relativeDir = path.dirname(this._buildSet.rootRelativeAbsolutePath(filename));
    
    const result = await this._buildSet.clang(['-MM', ...includeArgs, filename]);
    return `${relativeDir}/${result.stdout}`;
  }
}