#ifndef __CHUNKLANDS_LIBCXX_THREAD_HXX__
#define __CHUNKLANDS_LIBCXX_THREAD_HXX__

#include <thread>

namespace chunklands::libcxx {

class ThreadGuard {
public:
    static void DeclareMainThread()
    {
        main_thread_ = std::this_thread::get_id();
    }

    static void DeclareOpenGLThread()
    {
        opengl_thread_ = std::this_thread::get_id();
    }

    static bool IsMainThread()
    {
        return std::this_thread::get_id() == main_thread_;
    }

    static bool IsOpenGLThread()
    {
        return std::this_thread::get_id() == opengl_thread_;
    }

private:
    static std::thread::id main_thread_;
    static std::thread::id opengl_thread_;
};

}; // namespace chunklands::libcxx

#endif