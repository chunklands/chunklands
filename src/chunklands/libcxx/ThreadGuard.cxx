#include "ThreadGuard.hxx"

namespace chunklands::libcxx {
std::thread::id ThreadGuard::main_thread_;
std::thread::id ThreadGuard::opengl_thread_;
}