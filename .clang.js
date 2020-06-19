require('dotenv').config();
const os = require('os');
const clang = require('./clang');

const DEV = process.env.NODE_ENV !== 'production';

(async () => {
  const buildSet = new clang.BuildSet({
    clangfileAbsolutePath: __filename,
    rootAbsolutePath: __dirname,
    buildAbsolutePath: `${__dirname}/build`,
    debug: DEV,
    clangBin: process.env.CLANG_BIN,
    clangTidyBin: process.env.CLANG_TIDY_BIN
  })
  .addMakefileTarget('../deps/glfw/src/libglfw3.a', {
    cmd: 'cd ../deps/glfw'
      + ` && cmake -DGLFW_BUILD_DOCS=OFF -DGLFW_BUILD_TESTS=OFF -DGLFW_BUILD_EXAMPLES=OFF -DCMAKE_BUILD_TYPE=${DEV ? 'Debug' : 'Release'} --build .`
      + ' && make'
  });

  await new clang.CompileSet(buildSet, {std: 'c99', fPIC: true})
    .addSystemInclude('deps/glfw/deps')
    .addSystemSource('deps/glfw/deps/glad_gl.c')
    .addToBuildSet();

  await new clang.CompileSet(buildSet, {std: 'c++20', fPIC: true, shared: true})
    .addSystemInclude(
      'deps/boost_*/include',
      'deps/glfw/include',
      'deps/glfw/deps',
      'deps/glm',
      'deps/node/src',
      'deps/stb',
      'node_modules/node-addon-api',
    )
    .addInclude('src')
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
    .addLink(os.platform() === 'linux' ? 'X11' : null)
    .addMacOSFramework('CoreVideo', 'OpenGL', 'IOKit', 'Cocoa', 'Carbon')
    .addToBuildSet('chunklands.node');

  await buildSet.printMakefile('chunklands.node', process.stdout);
})();
