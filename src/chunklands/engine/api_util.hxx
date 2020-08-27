
#ifndef __CHUNKLANDS_ENGINE_APIUTIL_HXX__
#define __CHUNKLANDS_ENGINE_APIUTIL_HXX__

#include <boost/current_function.hpp>
#include <chunklands/libcxx/boost_thread.hxx>

#define _ENGINE_FN_NAME __api_fn_name
#define ENGINE_FN() static const char* _ENGINE_FN_NAME = BOOST_CURRENT_FUNCTION;

#define ENGINE_CHECK(x) ENGINE_CHECK_MSG((x), #x)
#define ENGINE_CHECK_MSG(x, MSG)                                       \
    do {                                                               \
        if (!(x)) {                                                    \
            return Err(create_engine_exception(_ENGINE_FN_NAME, MSG)); \
        }                                                              \
    } while (0)

namespace chunklands::engine {

template <class F, class R = std::result_of_t<F && ()>>
inline boost::future<R> EnqueueTask(boost::loop_executor& executor, F&& fn)
{

    boost::packaged_task<R()> task(std::forward<F>(fn));
    boost::future<R> result = task.get_future();
    executor.submit(std::move(task));

    return result;
}

} // namespace chunklands::engine

#endif