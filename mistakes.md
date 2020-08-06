Find it useful to show some silly mistakes:

# C++
- I `reinterpret_cast` a `void*` pointer from a specialized class pointer to its base class => segfaulting
- I negated an unsigned int
- I created the first clangjs concept with a Makefile without dependencies to the header files

# OpenGL
- I created a 1x1 pixel texture to test the shader, which worked. Then I replaced the texture with the real one and everything was black. => somehow there is no need for GL_TEXTURE_MIN_FILTER/GL_TEXTURE_MAG_FILTER for a 1x1 pixel texture, while other textures need it.

# JS
- I replaced gameloop `setImmediate` to `process.nextTick` and no worker results were processed