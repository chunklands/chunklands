Find it useful to show some silly mistakes:

# C++
- I `reinterpret_cast` a `void*` pointer from a specialized class pointer to its base class => segfaulting

# JS
- I replaced gameloop `setImmediate` to `process.nextTick` and no worker results were processed