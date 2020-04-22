#ifndef __CHUNKLANDS_CHECK_H__
#define __CHUNKLANDS_CHECK_H__

#include "error.h"
#include <string>

#define CHECK(X) do { if (!(X)) { \
    THROW_RT("failed: " #X); \
  }} while (0)

#define CHECK_MSG(X, MSG) do { if (!(X)) {  \
    std::string msg = "failed: " #X;        \
    msg += ", message: ";                   \
    msg += (MSG);                           \
    THROW_RT(msg);                          \
  }} while (0)

#endif
