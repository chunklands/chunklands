#ifndef __CHUNKLANDS_API_SHARED_HXX__
#define __CHUNKLANDS_API_SHARED_HXX__

#include "../Api.hxx"
#include <chunklands/libcxx/easy_profiler.hxx>
#include "../engine_exception.hxx"

#define _API_FN_NAME __api_fn_name
#define API_FN() static const char* _API_FN_NAME = BOOST_CURRENT_FUNCTION;

#define CHECK(x) do { if (!(x)) { BOOST_THROW_EXCEPTION(create_engine_exception(_API_FN_NAME, #x)); } } while(0)


namespace chunklands::engine {

  template<class C, class T>
  inline bool has_handle(const C& container, const T& element) {
    return container.find(element) != container.end();
  }

  template<class F, class R = std::result_of_t<F&&()>>
  inline boost::future<R> EnqueueTask(void* executor, F&& fn) {
    boost::loop_executor* e = reinterpret_cast<boost::loop_executor*>(executor);

    boost::packaged_task<R()> task(std::forward<F>(fn));
    boost::future<R> result = task.get_future();
    e->submit(std::move(task));

    return result;
  }

} // namespace chunklands::engine

#endif