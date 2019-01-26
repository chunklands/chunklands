# Chunklands

- [GitHub Issues](https://github.com/DaAitch/chunklands/issues)
- [GitHub Project](https://github.com/DaAitch/chunklands/projects/1)

## Technologies

![cmake](doc/cmake.svg)
![C++](doc/cpp.svg)
![NodeJS](doc/nodejs.svg)
![N-API](doc/napi.svg)
![Boost](doc/boost.svg)
![OpenGL](doc/opengl.svg)

## Progress

*This is a WIP game repository. Wait for it ...* :)

current status:

![Current Result](./status.png)

## Development

### Create new `ObjectWrap` classes

To reduce writing boilerplate, we can add new `ObjectWrap` with a small script.

```bash
node src-create-object-wrap.js MyNewClass
#  ✓ created '.../src/MyNewClass.js'
#  ✓ created '.../src/MyNewClassBase.h'
#  ✓ created '.../src/MyNewClassBase.cc'
#  ✓ added export to '.../src/module_exports.inl'
#  ✓ added include to '.../src/module_includes.inl'
```

It will not overwrite any file.
