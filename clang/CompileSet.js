
const os = require('os');
const path = require('path');
const util = require('./util');

module.exports = class CompileSet {
  /**
   * 
   * @param {import('./BuildSet')} buildSet 
   * @param {*} param1 
   */
  constructor(buildSet, {std, fPIC, shared, 'static': static_}) {
    this._includeDirGlobs = [];
    this._sourceGlobs = [];
    this._systemIncludeDirGlobs = [];
    this._systemSourceGlobs = [];

    this._includeDirs = [];
    this._sources = [];
    this._systemSources = [];
    this._libraries = [];
    this._buildLink = [];
    this._buildMacOSFrameworks = [];

    this._options = {
      std,
      fPIC,
      shared,
      'static': static_
    };
    this._buildSet = buildSet;
  }

  addInclude(...dirs) {
    for (const dir of dirs) {
      this._includeDirGlobs.push(dir);
    }

    return this;
  }

  addSystemInclude(...dirs) {
    for (const dir of dirs) {
      this._systemIncludeDirGlobs.push(dir);
    }

    return this;
  }

  addSource(...files) {
    for (const file of files) {
      this._sourceGlobs.push(file);
    }

    return this;
  }

  addSystemSource(...files) {
    for (const file of files) {
      this._systemSourceGlobs.push(file);
    }

    return this;
  }

  addLibrary(...files) {
    for (const file of files) {
      this._libraries.push(file);
    }

    return this;
  }

  addLink(...libs) {
    this._buildLink.push(...libs);
    return this;
  }

  addMacOSFramework(...frameworks) {
    this._buildMacOSFrameworks.push(...frameworks);
    return this;
  }

  async _prepare() {
    [
      this._sources,
      this._systemSources,
      this._includeDirs,
      this._systemIncludeDirs,
    ] = await Promise.all([
      this._buildSet.resolveGlobs(this._sourceGlobs),
      this._buildSet.resolveGlobs(this._systemSourceGlobs),
      this._buildSet.resolveGlobs(this._includeDirGlobs),
      this._buildSet.resolveGlobs(this._systemIncludeDirGlobs),
    ]);

    // -I dir1 -I dir2
    this._clangIncludeDirsArgs = [];
    for (const dir of this._includeDirs) {
      if (dir) {
        this._clangIncludeDirsArgs.push('-I', dir);
      }
    }

    // -isystem dir1 -isystem dir2
    this._clangSystemIncludeDirsArgs = [];
    for (const dir of this._systemIncludeDirs) {
      if (dir) {
        this._clangSystemIncludeDirsArgs.push('-isystem', dir);
      }
    }

    // -lgl -lX11
    this._clangLinkArgs = this._buildLink
      .filter(lib => lib)
      .map(lib => `-l${lib}`);

    // -framework Cocoa
    this._clangFrameworks = this._buildMacOSFrameworks
      .filter(framework => framework)
      .map(framework => `-framework ${framework}`);
  }

  getObjectPaths() {
    const deps = []
    for (const sources of [this._sources, this._systemSources]) {
      for (const source of sources) {
        deps.push(this._buildSet.rootRelative(util.sourceToObjectPath(source)));
      }
    }

    return deps;
  }

  _clangStdArg() {
    return this._options.std ? `-std=${this._options.std}` : null;
  }

  _clangfPICArg() {
    return this._options.fPIC ? '-fPIC' : null;
  }

  _clangWArgs(system) {
    if (system) {
      return []
    }

    return [
      '-Wall',
      '-Werror',
      '-Wextra',
    ];
  }

  _clangOArg() {
    return this._buildSet.debug ? '-O0' : '-Ofast'
  }

  _clangObjectCmd(source, system) {
    return [
      this._buildSet.clangBin,
      '-c',
      ...this._clangWArgs(system),
      this._clangfPICArg(),
      this._clangStdArg(),
      this._clangOArg(),
      ...this._clangIncludeDirsArgs,
      ...this._clangSystemIncludeDirsArgs,
      source,
      '-o',
      util.sourceToObjectPath(this._buildSet.rootRelative(source))
    ].filter(x => x).join(' ');
  }

  _clangUndefinedArg() {
    if (os.platform() !== 'darwin') {
      return null;
    }

    return '-undefined dynamic_lookup';
  }

  _clangTidyCmd(source) {
    return util.cleanArgs([
      this._buildSet.clangTidyBin,
      source,
      '--',
      this._clangStdArg(),
      ...this._clangIncludeDirsArgs,
      ...this._clangSystemIncludeDirsArgs,
    ]).join(' ');
  }

  _clangFrameworkArgs() {
    if (os.platform() !== 'darwin') {
      return [];
    }

    return this._clangFrameworks;
  }

  _clangCompileCmd(target, inputs) {
    return util.cleanArgs([
      this._buildSet.clangBin,
      this._options.shared ? '-shared' : null,
      this._options['static'] ? '-static' : null,
      this._clangUndefinedArg(),
      this._clangfPICArg(),
      this._clangStdArg(),
      this._clangOArg(),
      ...this._clangIncludeDirsArgs,
      ...this._clangSystemIncludeDirsArgs,
      ...inputs,
      ...this._libraries,
      ...this._clangLinkArgs,
      ...this._clangFrameworkArgs(),
      '-o',
      target
    ]).join(' ');
  }

  async addToBuildSet(resultTarget) {
    await this._prepare();

    const targets = [];

    for (const [system, sources] of [[false, this._sources], [true, this._systemSources]]) {
      for (const source of sources) {
        const targetWithDeps = await this._makefileTarget(source);
        const [target, depsString] = targetWithDeps.split(':', 2);
        const deps = depsString.trim().replace(/\s*\\\n\s*/gm, ' ').split(' ');

        const objectCmd = this._clangObjectCmd(source, system);

        this._buildSet.addMakefileTarget(this._buildSet.rootRelative(target), {
          normalDeps: deps,
          cmd: (system || this._buildSet.clangNoTidy)
            ? `@${objectCmd} && echo "\\033[1;32mOK\\033[0m  compile ${source}" || (echo "\\033[1;31mERR\\033[0m compile ${source}" && exit 1)`
            : `@(${this._clangTidyCmd(source)} || exit 1) && ${objectCmd} && echo "\\033[1;32mOK\\033[0m  compile+tidy ${source}" || (echo "\\033[1;31mERR\\033[0m compile+tidy ${source}" && exit 1)`
        });

        targets.push(this._buildSet.rootRelative(target));
      }
    }

    if (resultTarget) {
      const cmd = this._clangCompileCmd(resultTarget, targets);
      this._buildSet.addMakefileTarget(resultTarget, {
        normalDeps: [
          ...this.getObjectPaths(),
          ...this._libraries
        ],
        cmd: `@${cmd} && echo "\\033[1;32mOK  RESULT\\033[0m ${resultTarget}" || (echo "\\033[1;31mERR RESULT\\033[0m ${resultTarget}" && exit 1)`
      });
    }

    return this;
  }

  async _makefileTarget(filename) {
    const relativeDir = path.dirname(filename);
    
    const result = await this._buildSet.clang(util.cleanArgs([
      '-MM',
      this._clangStdArg(),
      ...this._clangSystemIncludeDirsArgs,
      ...this._clangIncludeDirsArgs,
      filename
    ]));

    return `${relativeDir}/${result.stdout}`;
  }
}

