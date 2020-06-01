Find it useful to show some silly mistakes:

# C++
- I `reinterpret_cast` a `void*` pointer from a specialized class pointer to its base class => segfaulting
- I negated an unsigned int

# JS
- I replaced gameloop `setImmediate` to `process.nextTick` and no worker results were processed