INCLUDE="\
  -I deps/boost_array/include \
  -I deps/boost_assert/include \
  -I deps/boost_bind/include \
  -I deps/boost_chrono/include \
  -I deps/boost_circular_buffer/include \
  -I deps/boost_concept_check/include \
  -I deps/boost_config/include \
  -I deps/boost_container_hash/include \
  -I deps/boost_core/include \
  -I deps/boost_detail/include \
  -I deps/boost_function/include \
  -I deps/boost_integer/include \
  -I deps/boost_iterator/include \
  -I deps/boost_move/include \
  -I deps/boost_mp11/include \
  -I deps/boost_mpl/include \
  -I deps/boost_optional/include \
  -I deps/boost_parameter/include \
  -I deps/boost_predef/include \
  -I deps/boost_preprocessor/include \
  -I deps/boost_ratio/include \
  -I deps/boost_signals2/include \
  -I deps/boost_smart_ptr/include \
  -I deps/boost_stacktrace/include \
  -I deps/boost_static_assert/include \
  -I deps/boost_system/include \
  -I deps/boost_throw_exception/include \
  -I deps/boost_type_index/include \
  -I deps/boost_type_traits/include \
  -I deps/boost_utility/include \
  -I deps/boost_variant/include \
  -I deps/glfw/include \
  -I deps/glfw/deps \
  -I deps/glm \
  -I deps/node/src \
  -I deps/stb \
  -I src \
  -I node_modules/node-addon-api \
"

clang \
  -c \
  -fPIC \
  -std=c99 \
  -I deps/glfw/deps \
  deps/glfw/deps/glad_gl.c \
  -o build/glad.o

# clang \
#   -shared \
#   -fPIC \
#   -std=c++20 \
#   $INCLUDE \
#   src/chunklands/js.cxx \
#   src/chunklands/math.cxx \
#   src/chunklands/stb.cxx \
#   src/chunklands/modules/init.cxx \
#   src/chunklands/modules/engine/engine_module.cxx \
#   src/chunklands/modules/game/game_module.cxx \
#   src/chunklands/modules/gl/gl_module.cxx \
#   src/chunklands/modules/misc/misc_module.cxx \
#   src/chunklands_node/chunklands_napimodule.cxx \
#   deps/boost_chrono/src/chrono.cpp \
#   deps/boost_chrono/src/process_cpu_clocks.cpp \
#   deps/boost_chrono/src/thread_clock.cpp \
#   build/glad.o \
#   deps/glfw/src/libglfw3.a \
#   -lX11 \
#   -o build/chunklands.node
  # -o build/libchunklands_generic.a

  
  
# clang \
#   -shared \
#   -fPIC \
#   -DPIC \
#   -std=c++20 \
#   $INCLUDE \
#   src/chunklands_node/chunklands_napimodule.cxx \
#   build/libchunklands_generic.a \
#   -o build/chunklands.node