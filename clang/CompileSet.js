
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
    this._buildRelativeIncludeDirGlobs = [];
    this._buildRelativeSourceGlobs = [];
    this._buildRelativeSystemIncludeDirGlobs = [];
    this._buildRelativeSystemSourceGlobs = [];

    this._buildRelativeIncludeDirs = [];
    this._buildRelativeSources = [];
    this._buildRelativeSystemSources = [];
    this._buildRelativeLibraries = [];
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

  addMacOSFramework(...frameworks) {
    this._buildMacOSFrameworks.push(...frameworks);
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
      if (dir) {
        this._clangIncludeDirsArgs.push('-I', dir);
      }
    }

    // -isystem dir1 -isystem dir2
    this._clangSystemIncludeDirsArgs = [];
    for (const dir of this._buildRelativeSystemIncludeDirs) {
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

  _getBuildRelativeObjectPaths() {
    const deps = []
    for (const sources of [this._buildRelativeSources, this._buildRelativeSystemSources]) {
      for (const source of sources) {
        deps.push(this._buildSet.rootRelative(util.sourceToObjectPath(source)));
      }
    }

    return deps;
  }

  getObjectPaths() {
    return this._getBuildRelativeObjectPaths().map(buildRelativePath => this._buildSet.rootRelative(buildRelativePath));
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
      ...this._buildRelativeLibraries,
      ...this._clangLinkArgs,
      ...this._clangFrameworkArgs(),
      '-o',
      target
    ]).join(' ');
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
          cmd: (system || this._buildSet.clangNoTidy)
            ? objectCmd
            : `${this._clangTidyCmd(source)} && ${objectCmd}`
        });

        targets.push(target);
      }
    }

    if (resultTarget) {
      this._buildSet.addMakefileTarget(resultTarget, {
        normalDeps: [
          ...this._getBuildRelativeObjectPaths(),
          ...this._buildRelativeLibraries
        ],
        cmd: this._clangCompileCmd(resultTarget, targets)
      });
    }

    return this;
  }

  async _makefileTarget(filename) {
    const relativeDir = path.dirname(this._buildSet.rootRelative(filename));
    
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

