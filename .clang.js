const clang = require('./clang');

(async () => {
  const bs = new clang.BuildSet({
    clangfileAbsolutePath: __filename,
    rootAbsolutePath: __dirname,
    buildAbsolutePath: `${__dirname}/build`
  });

  bs.addMakefileTarget('../deps/glfw/src/libglfw3.a', {
    cmd: 'cmake -DGLFW_BUILD_DOCS=OFF -DGLFW_BUILD_TESTS=OFF -DGLFW_BUILD_EXAMPLES=OFF --build ../deps/glfw && make -C ../deps/glfw'
  });

  await new clang.CompileSet(bs, {std: 'c99', fPIC: true})
    .addInclude('deps/glfw/deps')
    .addSource('deps/glfw/deps/glad_gl.c')
    .addToBuildSet();

  await new clang.CompileSet(bs, {std: 'c++20', fPIC: true, shared: true})
    .addInclude(
      'deps/boost_*/include',
      'deps/glfw/include',
      'deps/glfw/deps',
      'deps/glm',
      'deps/node/src',
      'deps/stb',
      'src',
      'node_modules/node-addon-api'
    )
    .addSource(
      'src/chunklands/**/*.cxx',
      'deps/boost_chrono/src/chrono.cpp',
      'deps/boost_chrono/src/process_cpu_clocks.cpp',
      'deps/boost_chrono/src/thread_clock.cpp',
      'src/chunklands_node/chunklands_napimodule.cxx'
    )
    .addLibrary(
      'deps/glfw/src/libglfw3.a',
      'build/deps/glfw/deps/glad_gl.o'
    )
    .addLink('X11')
    .addToBuildSet('chunklands.node');

  await bs.printMakefile('chunklands.node', process.stdout);
})();
