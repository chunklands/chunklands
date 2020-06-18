
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
    this._buildRelativeSystemIncludeDirGlobs = [];
    this._buildRelativeSystemSourceGlobs = [];

    this._buildRelativeIncludeDirs = [];
    this._buildRelativeSources = [];
    this._buildRelativeSystemSources = [];
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

  addSystemInclude(...dirs) {
    for (const dir of dirs) {
      this._buildRelativeSystemIncludeDirGlobs.push(this._buildSet.buildRelative(dir))
    }

    return this;
  }

  addSource(...files) {
    for (const file of files) {
      this._buildRelativeSourceGlobs.push(this._buildSet.buildRelative(file))
    }

    return this;
  }

  addSystemSource(...files) {
    for (const file of files) {
      this._buildRelativeSystemSourceGlobs.push(this._buildSet.buildRelative(file))
    }

    return this;
  }

  addLibrary(...files) {
    for (const file of files) {
      this._buildRelativeLibraries.push(this._buildSet.buildRelative(file));
    }

    return this;
  }

  addLink(...libs) {
    this._buildLink.push(...libs);
    return this;
  }

  async _prepare() {
    [
      this._buildRelativeSources,
      this._buildRelativeSystemSources,
      this._buildRelativeIncludeDirs,
      this._buildRelativeSystemIncludeDirs,
    ] = await Promise.all([
      this._buildSet.buildRelativeResolveGlobs(this._buildRelativeSourceGlobs),
      this._buildSet.buildRelativeResolveGlobs(this._buildRelativeSystemSourceGlobs),
      this._buildSet.buildRelativeResolveGlobs(this._buildRelativeIncludeDirGlobs),
      this._buildSet.buildRelativeResolveGlobs(this._buildRelativeSystemIncludeDirGlobs),
    ]);

    // -I dir1 -I dir2
    this._clangIncludeDirsArgs = [];
    for (const dir of this._buildRelativeIncludeDirs) {
      this._clangIncludeDirsArgs.push('-I', dir);
    }

    // -isystem dir1 -isystem dir2
    this._clangSystemIncludeDirsArgs = [];
    for (const dir of this._buildRelativeSystemIncludeDirs) {
      this._clangSystemIncludeDirsArgs.push('-isystem', dir);
    }

    // -lgl -lX11
    this._clangLinkArgs = this._buildLink.map(lib => `-l${lib}`)
  }

  _makefileProgDependencies() {
    const deps = []
    for (const sources of [this._buildRelativeSources, this._buildRelativeSystemSources]) {
      for (const source of sources) {
        deps.push(this._buildSet.rootRelativeAbsolutePath(util.sourceToObjectPath(source)));
      }
    }

    return deps;
  }

  _clangObjectCmd(source, system) {
    return [
      'clang',
      '-c',
      ...(system ? [] : [
        '-Wall',
        '-Werror',
        '-Wextra',
      ]),
      this._options.fPIC ? '-fPIC' : null,
      this._options.std ? `-std=${this._options.std}` : null,
      this._buildSet.debug ? '-O0' : '-Ofast',
      ...this._clangIncludeDirsArgs,
      ...this._clangSystemIncludeDirsArgs,
      source,
      '-o',
      util.sourceToObjectPath(this._buildSet.rootRelativeAbsolutePath(source))
    ].filter(x => x).join(' ');
  }

  _clangTidyCmd(source) {
    return [
      'clang-tidy',
      source,
      '--',
      this._options.std ? `-std=${this._options.std}` : null,
      ...this._clangIncludeDirsArgs,
      ...this._clangSystemIncludeDirsArgs,
    ].filter(x => x).join(' ');
  }

  _clangCompileCmd(target, inputs) {
    return [
      'clang',
      this._options.shared ? '-shared' : null,
      this._options.fPIC ? '-fPIC' : null,
      this._options.std ? `-std=${this._options.std}` : null,
      this._buildSet.debug ? '-O0' : '-Ofast',
      ...this._clangIncludeDirsArgs,
      ...this._clangSystemIncludeDirsArgs,
      ...inputs,
      ...this._buildRelativeLibraries,
      ...this._clangLinkArgs,
      '-o',
      target
    ].filter(x => x).join(' ');
  }

  async addToBuildSet(resultTarget) {
    await this._prepare();

    const targets = [];

    for (const [system, sources] of [[false, this._buildRelativeSources], [true, this._buildRelativeSystemSources]]) {
      for (const source of sources) {
        const targetWithDeps = await this._makefileTarget(source);
        const [target, depsString] = targetWithDeps.split(':', 2);
        const deps = depsString.trim().replace(/\s*\\\n\s*/gm, ' ').split(' ');

        const objectCmd = this._clangObjectCmd(source, system);

        this._buildSet.addMakefileTarget(target, {
          normalDeps: deps,
          cmd: (system || true)
            ? objectCmd
            : `${this._clangTidyCmd(source)} && ${objectCmd}`
        });

        targets.push(target);
      }
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

  async _makefileTarget(filename) {
    const relativeDir = path.dirname(this._buildSet.rootRelativeAbsolutePath(filename));
    
    const result = await this._buildSet.clang(['-MM', ...this._clangSystemIncludeDirsArgs, ...this._clangIncludeDirsArgs, filename]);
    return `${relativeDir}/${result.stdout}`;
  }
}