#ifndef __CHUNKLANDS_CHECK_H__
#define __CHUNKLANDS_CHECK_H__

#include <stdexcept>

#define CHECK(X) do { if (!(X)) { throw std::runtime_error("failed: " #X); }} while (0)

#endif
