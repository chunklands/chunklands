#ifndef __CHUNKLANDS_CONTAINER_OPEN_PRIO_QUEUE__
#define __CHUNKLANDS_CONTAINER_OPEN_PRIO_QUEUE__

#include <queue>

namespace chunklands {
  /**
   * It's not possible to access container of priority queue.
   * We make it happen.
   */
  template<typename T, typename Container = std::vector<T>,
	   typename Compare = std::less<typename Container::value_type>>
  class container_open_prio_queue : public std::priority_queue<T, Container, Compare> {
  public:
    Container& container() {
      return std::priority_queue<T, Container, Compare>::c;
    }

    const Container& container() const {
      return std::priority_queue<T, Container, Compare>::c;
    }
  };
}

#endif